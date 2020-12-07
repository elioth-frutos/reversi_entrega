#include <gtk/gtk.h>
#include "game.h"
#include "logic/logic.h"
#include "input_output/menu_io.h"
#include "input_output/game_io.h"

GtkBuilder *builder;
GtkWidget *window, *drawing_area, *event_box, *main_menu_window;
GtkWidget *user_cpu, *user_user, *cpu_itself, *cpu_cpu;

typedef struct {
    GtkWidget *w_txtvw_main;            // Pointer to text view object
    GtkWidget *w_dlg_file_choose;       // Pointer to file chooser dialog box
    GtkTextBuffer *textbuffer_main;     // Pointer to text buffer
    GtkWidget *w_dlg_about;             // Pointer to about dialog box
} app_widgets;

int main(int argc, char **argv)
{
    app_widgets *widgets = g_slice_new(app_widgets);

    // Create a singly-linked list to store a radio button group.
    GSList *list = NULL;

    user_cpu =
        GTK_WIDGET(gtk_builder_get_object(builder, "user_cpu"));

    user_user =
        GTK_WIDGET(gtk_builder_get_object(builder, "user_user"));

    cpu_itself =
        GTK_WIDGET(gtk_builder_get_object(builder, "cpu_itself"));

    /*
    cpu_cpu =
        GTK_WIDGET(gtk_builder_get_object(builder, "cpu_cpu"));
    */

    list = g_slist_append (list, &user_cpu);
    list = g_slist_append (list, &user_user);
    list = g_slist_append (list, &cpu_itself);
    //list = g_slist_append (list, &cpu_cpu);

    // Create the radio button group.
    gtk_radio_button_set_group(cpu_cpu, list);

    // Declare a game struct to store the state of the game.
    Game game;
    // Initialize the game board.
    initialize_board(game.board, 0, 0);

    // Initialize everything needed to operate the toolkit.
    gtk_init(&argc, &argv);

    // Get the builder.
    builder = gtk_builder_new_from_file("src/glade_files/GUI.glade");

    // Get the main window.
    window = GTK_WIDGET(gtk_builder_get_object(builder, "main_window"));

    // Get the main menu window.
    main_menu_window =
        GTK_WIDGET(gtk_builder_get_object(builder, "main_menu_window"));

    // Get the event box.
    event_box = GTK_WIDGET(gtk_builder_get_object(builder, "event_box"));
    // Connect the button-press signal to the corresponding callback.
    g_signal_connect(
            event_box, "button-press-event",
            G_CALLBACK(button_pressed_callback), &game);

    // Get the drawing area.
    drawing_area =
        GTK_WIDGET(gtk_builder_get_object(builder, "drawing_area"));
    // Connect the draw signal to the drawing callback function.
    g_signal_connect(
            drawing_area, "draw",
            G_CALLBACK(draw_callback), &game);

    // Get pointers to widgets
    widgets->w_txtvw_main = GTK_WIDGET(gtk_builder_get_object(builder, "txtvw_main"));
    widgets->w_dlg_file_choose = GTK_WIDGET(gtk_builder_get_object(builder, "dlg_file_choose"));
    widgets->textbuffer_main = GTK_TEXT_BUFFER(gtk_builder_get_object(builder, "textbuffer_main"));
    widgets->w_dlg_about = GTK_WIDGET(gtk_builder_get_object(builder, "dlg_about"));
    
    gtk_builder_connect_signals(builder, widgets);


    // Show the main window and it's child widgets.
    gtk_widget_show_all((GtkWidget *) window);

    // Show the main menu window on application start.
    gtk_widget_show_all((GtkWidget *) main_menu_window);

    // Initialize the game state to main_menu.
    game.state = main_menu;
    game.builder = builder;
    game.drawing_area = drawing_area;

    // Run GTK.
    gtk_main();

    g_slice_free(app_widgets, widgets);

    return 0;
}

// Help --> About
void on_menuitm_about_activate(GtkMenuItem *menuitem, app_widgets *app_wdgts)
{
    gtk_widget_show(app_wdgts->w_dlg_about);
}

// About dialog box Close button
void on_dlg_about_response(GtkDialog *dialog, gint response_id, app_widgets *app_wdgts)
{
    gtk_widget_hide(app_wdgts->w_dlg_about);
}

// Called when window is closed
void on_main_window_destroy(void)
{
    gtk_main_quit();
}

// Called when the "quit" menu item is activated.
void on_menu_item_quit_activate(void)
{
    gtk_main_quit();
}
