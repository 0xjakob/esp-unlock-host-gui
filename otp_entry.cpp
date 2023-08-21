#include <string>
#include "otp_entry.hpp"
#include "totp_command.hpp"
    
OTP_Entry::OTP_Entry(GtkContainer* cont,
        const std::string_view& entry_name,
        std::shared_ptr<CommandReader> command_reader) 
        : container(cont), 
        name(entry_name.data(), entry_name.size()), 
        cmd_reader(command_reader) {
    button = gtk_button_new_with_label(name.c_str());
    password_output = gtk_entry_new();
    layout = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);

    gtk_container_add(GTK_CONTAINER(container), layout);
    gtk_box_set_homogeneous(reinterpret_cast<GtkBox*>(layout), TRUE);
    
    gtk_container_add(GTK_CONTAINER(layout), button);
    gtk_container_add(GTK_CONTAINER(layout), password_output);

    gtk_editable_set_editable((GtkEditable*) password_output, false);
    
    g_signal_connect(button, "clicked", G_CALLBACK(on_button_clicked), this);
}

OTP_Entry::~OTP_Entry() {
    gtk_container_remove(GTK_CONTAINER(layout), password_output);
    gtk_container_remove(GTK_CONTAINER(layout), button);
    gtk_container_remove(GTK_CONTAINER(container), layout);
} 

void OTP_Entry::on_button_clicked(GtkButton* button, gpointer data) {
    OTP_Entry* instance = static_cast<OTP_Entry*>(data);
    instance->on_button_clicked_member();
}
    
void OTP_Entry::on_button_clicked_member() {
    std::string result;
    try {
        std::shared_ptr<TOTP_Command> cmd_ate(new TOTP_Command(name));
        cmd_reader->execute_cmd(cmd_ate);
        result = cmd_ate->get_result().get();
        while (result.back() == '\r' || result.back() == '\n') {
            result.pop_back();
        }
        gtk_entry_set_text(reinterpret_cast<GtkEntry*>(password_output), result.c_str());
//        gtk_label_set_text(info_bar, "");
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
