#include <memory>
#include <string>
#include <gtk/gtk.h>
#include "gtk_widget.hpp"
#include "command_reader.hpp"
#include <semaphore>

class OTP_EntryAddWindow : public GTK_Widget {
public:
    OTP_EntryAddWindow(GApplication* app, std::shared_ptr<CommandReader> command_reader, std::binary_semaphore* sem);
    ~OTP_EntryAddWindow() override;
private:
    static void on_button_clicked(GtkButton* btn, gpointer data);
    void on_button_clicked_member();
    GtkWidget* window;
    GtkWidget* button;
    GtkWidget* two_entry_layout;
    GtkWidget* name_entry;
    GtkWidget* secret_entry;
    GtkWidget* status_label;
    GtkWidget* main_layout;
    std::shared_ptr<CommandReader> cmd_reader;
    std::binary_semaphore* notification_sem;

    /**
     * A successful addition of a new key will destroy this window and so will the destructor.
     * To prevent double-destruction of this window, we need a flag.
     */
    bool destroyed;
};
