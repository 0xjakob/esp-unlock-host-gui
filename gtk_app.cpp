#include <gtk/gtk.h>
#include <map>
#include <string>
#include <iostream>
#include <semaphore>
#include <thread>
#include <chrono>
#include <stop_token>
#include "command_reader.hpp"
#include "hash_command.hpp"
#include "totp_command.hpp"
#include "totp_list_command.hpp"

#include "gtk_widget.hpp"
#include "otp_entry.hpp"
#include "otp_add_entry.hpp"
#include "gtk_app.hpp"

using namespace std;

namespace {

/**
 * This is the a symlink that is automatically created by a udev-rule from the package 
 * (see udev rule in package for more information).
 * For testing, you can change this to /dev/ttyACM<n> or whatever the generic name of your ACM
 * driver is.
 */
const char* DEVICE = "/dev/esp-unlock";

enum class AppState {
    NOT_CONNECTED,
    CONNECTED,
    CLOSING
};

map<string, GtkWidget*> widget_registry;

void color_text(string& text, const string& color) {
    text.insert(0, "<span background=\"" + color + "\">");
    text.append("</span>");
}

GApplication* g_app;
std::stop_source g_stop_source;

void on_destroy(GApplication* app, gpointer data) {
    printf("destroy cb called\n");
    g_stop_source.request_stop();
}

void on_app_activate(GApplication* app, gpointer data) {
    g_app = app;
    GtkWidget* window = gtk_application_window_new(GTK_APPLICATION(app));
    gtk_window_resize((GtkWindow*) window, 300, 400);
    g_signal_connect(window, "destroy", G_CALLBACK(on_destroy), NULL);

    GtkWidget* layout = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
//    gtk_flow_box_set_row_spacing((GtkFlowBox*) layout, 10);

    // We are using a simple label for now instead of InfoBar since it seems easier.
    GtkWidget* info_bar = gtk_label_new(nullptr);
    gtk_label_set_text(reinterpret_cast<GtkLabel*>(info_bar), "No device");

    gtk_container_add(GTK_CONTAINER(layout), info_bar);
    gtk_container_add(GTK_CONTAINER(window), layout);

    widget_registry["window"] = window;
    widget_registry["layout"] = layout;
    widget_registry["info_bar"] = info_bar;

    gtk_widget_show_all(GTK_WIDGET(window));
    printf("shown all initial widgets\n");
}

std::binary_semaphore device_found(0);
std::list<std::string> key_names;
std::shared_ptr<CommandReader> cmd_reader;
std::list<std::shared_ptr<GTK_Widget> > otp_entries;
AppState app_state = AppState::NOT_CONNECTED;

void update_totp_names(std::list<std::string>& totp_key_names) {
    std::shared_ptr<TOTP_ListCommand> list_cmd(new TOTP_ListCommand());
    cmd_reader->execute_cmd(list_cmd);
    totp_key_names = list_cmd->get_result().get();
}

/**
 * Check for the ESP-Unlock hardware token
 */
void check_for_esp_unlock() {
    printf("started checker\n");
    bool found = false;
    std::string result;

    while (!found) {
//        if (stop_semaphore->try_acquire()) {
        if (g_stop_source.stop_requested()) {
            return;
        }

//        GtkLabel* info_bar = (GtkLabel*) widget_registry["info_bar"];
        try {
            cmd_reader = make_shared<CommandReader>(DEVICE);
            found = true;
            result = "Device found";
            color_text(result, "green");
//            gtk_label_set_markup(info_bar, result.c_str());
        } catch (ReaderException& e) {
            // seemingly, EACCESS can happen once device has just been plugged in
            if (e.get_error_number() == ENOENT || e.get_error_number() == EACCES) {
                printf("not found, searching...\n");
                std::this_thread::sleep_for(std::chrono::seconds(1));
                continue; // user could insert the device later
            } else {
                if (e.get_error_number() == EPERM) {
                    result.append("ERROR: No permission to open device.");
                } else {
                    printf("error_number: %d\n", e.get_error_number());
                    result.append("ERROR: Unkown error.");
                }
                color_text(result, "yellow");
                printf("%s", result.c_str());
//                gtk_label_set_markup(info_bar, result.c_str());
//                break;
            }
        } catch (invalid_argument e) {
            result = e.what();
            color_text(result, "yellow");
            printf("%s", result.c_str());
//            gtk_label_set_markup(info_bar, result.c_str());
            break;
        } catch (out_of_range e) {
            result = e.what();
            color_text(result, "yellow");
            printf("%s", result.c_str());
//            gtk_label_set_markup(info_bar, result.c_str());
            break;
        }
    }
    
    if (found) {
        printf("device found!\n");
        try {
            update_totp_names(key_names);
        } catch (ReaderException& e) {
            throw; // TODO: exit gracefully
        }
        device_found.release();
    } else {
        printf("ERROR while trying to find the device\n");
        // TODO: exit gracefully
    }
}

gboolean search_hook(gpointer user_data) {
    if (device_found.try_acquire()) {
        printf("got signal\n");
        if (app_state == AppState::CONNECTED) {
            printf("updating key names\n");
            try {
                update_totp_names(key_names);
            } catch (ReaderException& e) {
                throw; // TODO: exit gracefully
            }
        }
        otp_entries.clear();
        printf("adding widgets, key names:");
        for (auto name: key_names) {
            printf("%s, ", name.c_str());
        }
        printf("\n");
    
        GtkWidget* layout = widget_registry["layout"];
        for (auto name: key_names) {
            otp_entries.push_back(make_shared<OTP_Entry>(GTK_CONTAINER(layout), std::string_view(name), cmd_reader));
        }
    
        otp_entries.push_back(make_shared<OTP_AddEntry>(g_app, GTK_CONTAINER(layout), cmd_reader, &device_found));
        
        GtkLabel* info_bar = (GtkLabel*) widget_registry["info_bar"];
        string device_found_text("Device Found");
        color_text(device_found_text, "lightblue");
        gtk_label_set_markup(info_bar, device_found_text.c_str());
    
        gtk_widget_show_all(GTK_WIDGET(layout));
        app_state = AppState::CONNECTED;
        printf("done\n");
    } else {
        if (g_stop_source.stop_requested()) {
            return false;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    return true;
}

}

int run_gtk_app(int argc, char *argv[]) {
    /*
     * Since version 2.74, the default application flags change
     * from G_APPLICATION_FLAGS_NONE to G_APPLICATION_DEFAULT_FLAGS,
     * while G_APPLICATION_FLAGS_NONE are deprecated immediately.
     * To prevent a compiler warning with newer glib versions, we have this switch.
     */
#if GLIB_CHECK_VERSION(2, 74, 0)
    GApplicationFlags app_flags = G_APPLICATION_DEFAULT_FLAGS;
#else
    GApplicationFlags app_flags = G_APPLICATION_FLAGS_NONE;
#endif
    GtkApplication* app = gtk_application_new(nullptr, app_flags);
    g_signal_connect(app, "activate", G_CALLBACK(on_app_activate), NULL);
    g_idle_add(search_hook, nullptr);
    std::jthread device_finder(check_for_esp_unlock);
    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    return status;
}
