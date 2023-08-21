#include <memory>
#include <string>
#include <gtk/gtk.h>
#include "gtk_widget.hpp"
#include "command_reader.hpp"

class OTP_Entry : public GTK_Widget {
public:
    OTP_Entry(GtkContainer* cont, const std::string_view& entry_name, std::shared_ptr<CommandReader> command_reader);
    ~OTP_Entry() override;
private:
    static void on_button_clicked(GtkButton* btn, gpointer data);
    void on_button_clicked_member();
    GtkContainer* container;
    GtkWidget* button;
    GtkWidget* password_output;
    GtkWidget* layout;
    std::shared_ptr<CommandReader> cmd_reader;
    std::string name;
};


