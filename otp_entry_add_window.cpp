#include <string>
#include "otp_entry_add_window.hpp"
#include "totp_add_command.hpp"
#include "totp_command_exception.hpp"

namespace {

const char* ERR_INVALID_INPUT = "<span background=\"yellow\">Invalid input</span>";
const char* ERR_NOPERM = "<span background=\"red\">ERROR: No permission to open device</span>";
const char* ERR_NODEV = "<span background=\"red\">ERROR: Device does not exist</span>";
const char* ERR_UNKNOWN = "<span background=\"red\">ERROR: unknown error</span>";

}
    
OTP_EntryAddWindow::OTP_EntryAddWindow(GApplication* app, std::shared_ptr<CommandReader> command_reader, std::binary_semaphore* sem)
        : cmd_reader(command_reader),
        notification_sem(sem),
        destroyed(false) {
    window = gtk_application_window_new(GTK_APPLICATION(app));
//    gtk_window_resize((GtkWindow*) window, 300, 400);
    main_layout = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    two_entry_layout = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    button = gtk_button_new_with_label("Add Entry");
    name_entry = gtk_entry_new();
    secret_entry = gtk_entry_new();
    status_label = gtk_label_new(nullptr);

    gtk_entry_set_placeholder_text(reinterpret_cast<GtkEntry*>(name_entry), "name of key");
    gtk_entry_set_placeholder_text(reinterpret_cast<GtkEntry*>(secret_entry), "key (base32 encoded)");

    gtk_container_add(GTK_CONTAINER(window), main_layout);
    gtk_container_add(GTK_CONTAINER(main_layout), two_entry_layout);
    gtk_box_set_homogeneous(reinterpret_cast<GtkBox*>(two_entry_layout), TRUE);
    
    gtk_container_add(GTK_CONTAINER(two_entry_layout), name_entry);
    gtk_container_add(GTK_CONTAINER(two_entry_layout), secret_entry);
    gtk_container_add(GTK_CONTAINER(main_layout), two_entry_layout);
    gtk_container_add(GTK_CONTAINER(main_layout), button);
    gtk_container_add(GTK_CONTAINER(main_layout), status_label);

    g_signal_connect(button, "clicked", G_CALLBACK(on_button_clicked), this);

    gtk_widget_show_all(GTK_WIDGET(window));
}

OTP_EntryAddWindow::~OTP_EntryAddWindow() {
    if (!destroyed) {
        gtk_widget_destroy(window);
    }
} 

void OTP_EntryAddWindow::on_button_clicked(GtkButton* button, gpointer data) {
    OTP_EntryAddWindow* instance = static_cast<OTP_EntryAddWindow*>(data);
    instance->on_button_clicked_member();
}
    
void OTP_EntryAddWindow::on_button_clicked_member() {
    std::string result;
    try {
        std::string name(gtk_entry_get_text(reinterpret_cast<GtkEntry*>(name_entry)));
        std::string secret(gtk_entry_get_text(reinterpret_cast<GtkEntry*>(secret_entry)));
        printf("writing name: %s, secret: %s\n", name.c_str(), secret.c_str());
        std::shared_ptr<TOTP_AddCommand> cmd_add(new TOTP_AddCommand(name.c_str(), secret.c_str()));
        cmd_reader->execute_cmd(cmd_add);
        std::future<void> waiter = cmd_add->get_result();
        waiter.get();
        printf("added secret\n");
        gtk_widget_destroy(window);
        destroyed = true;
        notification_sem->release();
    } catch (TOTP_CMD_Exception e) {
        gtk_label_set_markup((GtkLabel*) status_label, ERR_INVALID_INPUT);
    } catch (std::invalid_argument e) {
        result = e.what();
        printf("%s\n", result.c_str());
//        color_text(result, "yellow");
//        gtk_label_set_markup(info_bar, result.c_str());
    } catch (std::out_of_range e) {
        result = e.what();
        printf("%s\n", result.c_str());
//        color_text(result, "yellow");
//        gtk_label_set_markup(info_bar, result.c_str());
    } catch (ReaderException e) {
        result = std::string("ERROR: ").append(e.what());
        result.append(".");
        if (e.get_error_number() == EPERM) {
            gtk_label_set_markup((GtkLabel*) status_label, ERR_NOPERM);
        } else if (e.get_error_number() == ENOENT) {
            gtk_label_set_markup((GtkLabel*) status_label, ERR_NODEV);
        } else {
            gtk_label_set_markup((GtkLabel*) status_label, ERR_UNKNOWN);
            printf("Readerexception with ERRNO: %d\n", e.get_error_number());
        }
    }
}
