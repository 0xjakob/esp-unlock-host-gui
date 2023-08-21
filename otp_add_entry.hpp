#include <memory>
#include <string>
#include <gtk/gtk.h>
#include "gtk_widget.hpp"
#include "command_reader.hpp"
#include "otp_entry_add_window.hpp"

class OTP_AddEntry : public GTK_Widget {
public:
    OTP_AddEntry(GApplication* app, GtkContainer* cont, std::shared_ptr<CommandReader> command_reader, std::binary_semaphore* sem);
    ~OTP_AddEntry() override;
private:
    static void on_button_clicked(GtkButton* btn, gpointer data);
    void on_button_clicked_member();
    GtkContainer* container;
    GtkWidget* button;

    /**
     * Input for inputting the secret in base32 format.
     */
    GtkWidget* placeholder_label;

    GApplication* app;
    GtkWidget* layout;
    std::shared_ptr<CommandReader> cmd_reader;
    std::shared_ptr<OTP_EntryAddWindow> add_window;
    std::binary_semaphore* notification_sem;
};


