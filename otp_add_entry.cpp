#include <string>
#include <semaphore>
#include "otp_add_entry.hpp"
    
OTP_AddEntry::OTP_AddEntry(GApplication* application,
        GtkContainer* cont,
        std::shared_ptr<CommandReader> command_reader,
        std::binary_semaphore* sem) 
        : app(application),
        container(cont), 
        cmd_reader(command_reader),
        add_window(),
        notification_sem(sem) {
    button = gtk_button_new_with_label("ADD NEW");
    placeholder_label = gtk_label_new(nullptr);
    layout = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);

    gtk_container_add(GTK_CONTAINER(container), layout);
    gtk_box_set_homogeneous(reinterpret_cast<GtkBox*>(layout), TRUE);
    
    gtk_container_add(GTK_CONTAINER(layout), button);
    gtk_container_add(GTK_CONTAINER(layout), placeholder_label);
    
    g_signal_connect(button, "clicked", G_CALLBACK(on_button_clicked), this);
}

OTP_AddEntry::~OTP_AddEntry() {
    gtk_container_remove(GTK_CONTAINER(layout), placeholder_label);
    gtk_container_remove(GTK_CONTAINER(layout), button);
    gtk_container_remove(GTK_CONTAINER(container), layout);
} 

void OTP_AddEntry::on_button_clicked(GtkButton* button, gpointer data) {
    OTP_AddEntry* instance = static_cast<OTP_AddEntry*>(data);
    instance->on_button_clicked_member();
}
    
void OTP_AddEntry::on_button_clicked_member() {
    std::string result;
    try {
        printf("Adding new entry...\n");
        add_window = make_shared<OTP_EntryAddWindow>(app, cmd_reader, notification_sem);
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
            result.append(" No permission to open device.");
        } else if (e.get_error_number() == ENOENT) {
            result.append(" Device doesn't exist.");
        } else {
            result.append(" Unknown error: ");
            result.append(std::to_string(e.get_error_number()));
        }
        printf("result: %s\n", result.c_str());
//        color_text(result, "yellow");
//        gtk_label_set_markup(info_bar, result.c_str());
    }
}
