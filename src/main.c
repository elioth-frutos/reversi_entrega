#include <gtk/gtk.h>
#include "game.h"
#include "logic/logic.h"
#include "input_output/menu_io.h"
#include "input_output/game_io.h"

GtkBuilder *builder;
GtkWidget *window, *drawing_area, *event_box, *main_menu_window,
          *game_over_window, *main_menu_ok_button,
          *who_plays_first_box, *select_color_label,
          *single_player_radio, *two_players_radio,
          *cpu_itself_radio, *cpu_cpu_radio,
          *player_name_label, *opponents_name_label,
          *who_plays_first_select_random_radio,
          *player_1_first_radio, *player_2_first_radio,
          *select_color_random_radio,
          *player_1_white_radio, *player_1_black_radio,
          *turn_info_label, *score_info_label,
          *game_over_label,
          *game_over_start_new_game, *game_over_quit_game,
          *statistics_window, *view_statistics_button,
          *view_statistics_label, *statistics_text_view,
          *statistics_close_button,
          *load_game_menubar_button, *save_game_menubar_button,
          *file_chooser,
          *file_chooser_load_game_button, *file_chooser_cancel_button;

// Statistics text buffer.
GtkTextBuffer *buffer;

// Variables to store the names of the players.
gchar *player_name, *opponents_name;

typedef struct {
    GtkWidget *w_txtvw_main;            // Pointer to text view object
    GtkWidget *w_dlg_file_choose;       // Pointer to file chooser dialog box
    GtkTextBuffer *textbuffer_main;     // Pointer to text buffer
    GtkWidget *w_dlg_about;             // Pointer to about dialog box
} app_widgets;

// Function declarations.
// When the "start game" button is pressed.
void on_main_menu_ok_button_button_press_event(GtkWidget *button, Game *game);

// Callbacks for the radio buttons that set the game mode.
void on_user_cpu_toggled(GtkWidget *button, Game *game);
void on_user_user_toggled(GtkWidget *button, Game *game);
void on_cpu_itself_toggled(GtkWidget *button, Game *game);
void on_cpu_cpu_toggled(GtkWidget *button, Game *game);

// Callbacks for the radio buttons that determine who plays first.
static void select_first_player_randomly(GtkWidget *button, Game *game);
void on_player_1_first_toggled(GtkWidget *button, Game *game);
void on_player_2_first_toggled(GtkWidget *button, Game *game);

// Callbacks for the radio buttons that set the color of player 1.
static void select_color_randomly(GtkWidget *button, Game *game);
void on_player_1_white_toggled(GtkWidget *button, Game *game);
void on_player_1_black_toggled(GtkWidget *button, Game *game);

// Callbacks for the buttons on the game over window.
void on_game_over_start_new_game_clicked(GtkWidget *button, Game *game);
void on_quit_game_clicked(GtkWidget *button, Game *game);

// Callback for the statistics window.
static void statistics_callback(GtkWidget *button);
static void statistics_close(void);

static void load_game_callback(void);
static void save_game_callback(void);
static void filechooser_load_game(void);
static void filechooser_close(void);

int main(int argc, char **argv)
{
    // Allocate memory for the about dialog.
    app_widgets *widgets = g_slice_new(app_widgets);

    // Declare a game struct to store the state of the game.
    Game game;

    // Initialize everything needed to operate the toolkit.
    gtk_init(&argc, &argv);

    // Get the builder.
    builder = gtk_builder_new_from_file("src/glade_files/GUI.glade");

    // Get the main window.
    window = GTK_WIDGET(gtk_builder_get_object(builder, "main_window"));

    // Get the main menu window.
    main_menu_window =
        GTK_WIDGET(gtk_builder_get_object(builder, "main_menu_window"));

    // Get the game over window.
    game_over_window =
        GTK_WIDGET(gtk_builder_get_object(builder, "game_over_window"));

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

    // Get the GtkLabels that show the game information and status.
    turn_info_label = 
        GTK_WIDGET(gtk_builder_get_object(builder, "turn_info_label"));
    score_info_label = 
        GTK_WIDGET(gtk_builder_get_object(builder, "score_info_label"));

    // Get pointers to widgets
    widgets->w_txtvw_main =
        GTK_WIDGET(gtk_builder_get_object(builder, "txtvw_main"));
    widgets->w_dlg_file_choose =
        GTK_WIDGET(gtk_builder_get_object(builder, "dlg_file_choose"));
    widgets->textbuffer_main =
        GTK_TEXT_BUFFER(gtk_builder_get_object(builder, "textbuffer_main"));
    widgets->w_dlg_about =
        GTK_WIDGET(gtk_builder_get_object(builder, "dlg_about"));
    
    gtk_builder_connect_signals(builder, widgets);

    // Main menu window.
    // Get the main menu's "ok" button.
    main_menu_ok_button =
        GTK_WIDGET(gtk_builder_get_object(builder, "main_menu_ok_button"));
    // Connect the "clicked" signal with its signal handler.
    g_signal_connect(
            main_menu_ok_button, "clicked",
            G_CALLBACK(on_main_menu_ok_button_button_press_event), &game);

    // Get the "single player" radio button.
    single_player_radio =
        GTK_WIDGET(gtk_builder_get_object(builder, "user_cpu"));
    // Set this button as active by default.
    gtk_toggle_button_set_active(single_player_radio, TRUE);
    // Connect the "toggled" signal for the single player radio button
    // with its corresponding signal handler.
    g_signal_connect(
            single_player_radio, "toggled",
            G_CALLBACK(on_user_cpu_toggled), &game);

    // Get the "two players" radio button.
    two_players_radio =
        GTK_WIDGET(gtk_builder_get_object(builder, "user_user"));
    // Connect the "toggled" signal for the two players radio button
    // with its corresponding signal handler.
    g_signal_connect(
            two_players_radio, "toggled",
            G_CALLBACK(on_user_user_toggled), &game);

    // Get the "CPU vs itself" radio button.
    cpu_itself_radio =
        GTK_WIDGET(gtk_builder_get_object(builder, "user_itself"));
    // Connect the "toggled" signal for the "CPU vs itself" radio button
    // with its corresponding signal handler.
    g_signal_connect(
            cpu_itself_radio, "toggled",
            G_CALLBACK(on_cpu_itself_toggled), &game);

    // Get the "CPU vs another CPU" radio button.
    cpu_cpu_radio =
        GTK_WIDGET(gtk_builder_get_object(builder, "cpu_cpu"));
    // Connect the "toggled" signal for the "CPU vs CPU" radio button
    // with its corresponding signal handler.
    g_signal_connect(
            cpu_cpu_radio, "toggled",
            G_CALLBACK(on_cpu_cpu_toggled), &game);

    // Get the button to randomly select the first player.
    who_plays_first_select_random_radio =
        GTK_WIDGET(gtk_builder_get_object(
                    builder, "who_plays_first_select_random_radio"));
    // Connect the "toggled" signal for the button that selects the first
    // player randomly with its corresponding signal handler.
    g_signal_connect(
            who_plays_first_select_random_radio, "toggled",
            G_CALLBACK(select_first_player_randomly), &game);

    // Get the "player 1 first" radio button.
    player_1_first_radio =
        GTK_WIDGET(gtk_builder_get_object(builder, "player_1_first"));
    // Set this button as active by default.
    gtk_toggle_button_set_active(player_1_first_radio, TRUE);
    // Connect the "toggled" signal for the "player 1 first" radio button
    // with its corresponding signal handler.
    g_signal_connect(
            player_1_first_radio, "toggled",
            G_CALLBACK(on_player_1_first_toggled), &game);

    // Get the "player 2 first" radio button.
    player_2_first_radio =
        GTK_WIDGET(gtk_builder_get_object(builder, "player_2_first"));
    // Connect the "toggled" signal for the "player 2 first" radio button
    // with its corresponding signal handler.
    g_signal_connect(
            player_2_first_radio, "toggled",
            G_CALLBACK(on_player_2_first_toggled), &game);

    // Get the button to randomly assign a color to each player.
    select_color_random_radio =
        GTK_WIDGET(gtk_builder_get_object(
                    builder, "select_color_random_radio"));
    // Connect the "toggled" signal for the button that assigns the color
    // randomly with its corresponding signal handler.
    g_signal_connect(
            select_color_random_radio, "toggled",
            G_CALLBACK(select_color_randomly), &game);

    // Get the "player 1 plays white" radio button.
    player_1_white_radio =
        GTK_WIDGET(gtk_builder_get_object(builder, "player_1_white"));
    // Set this button as active by default.
    gtk_toggle_button_set_active(player_1_white_radio, TRUE);
    // Connect the "toggled" signal for the "player 1 plays white" radio
    // button with its corresponding signal handler.
    g_signal_connect(
            player_1_white_radio, "toggled",
            G_CALLBACK(on_player_1_white_toggled), &game);

    // Get the "player 1 plays black" radio button.
    player_1_black_radio =
        GTK_WIDGET(gtk_builder_get_object(builder, "player_1_black"));
    // Connect the "toggled" signal for the "player 1 plays black" radio
    // button with its corresponding signal handler.
    g_signal_connect(
            player_1_black_radio, "toggled",
            G_CALLBACK(on_player_1_black_toggled), &game);

    // Game over window.
    // Get the game over label.
    game_over_label =
        GTK_WIDGET(gtk_builder_get_object(builder, "game_over_label"));
    // Get the button to start a new game on the "game over" window.
    game_over_start_new_game =
        GTK_WIDGET(gtk_builder_get_object(builder, "game_over_start_new_game"));
    // Connect the "clicked" signal for the button to start a new game
    // from the "game over" window to its corresponding handler.
    g_signal_connect(
            game_over_start_new_game, "clicked",
            G_CALLBACK(on_game_over_start_new_game_clicked), &game);

    // Get the button to quit the game on the "game over" window.
    game_over_quit_game =
        GTK_WIDGET(gtk_builder_get_object(builder, "quit_game"));
    // Connect the "clicked" signal for the button to quit the game
    // from the "game over" window to its corresponding handler.
    g_signal_connect(
            game_over_quit_game, "clicked",
            G_CALLBACK(on_quit_game_clicked), &game);

    // Statistics window.
    // Get the statistics window.
    statistics_window =
        GTK_WIDGET(gtk_builder_get_object(builder, "statistics_window"));

    // Get the close button from the statistics window.
    statistics_close_button =
        GTK_WIDGET(gtk_builder_get_object(builder, "statistics_close_button"));
    // Connect the button-press-event signal with the callback function that
    // closes the statistics window.
    g_signal_connect(
            statistics_close_button, "button-press-event",
            G_CALLBACK(statistics_close), NULL);

    // "View statistics" menu button.
    view_statistics_button =
        GTK_WIDGET(gtk_builder_get_object(builder, "view_statistics_button"));
    // Connect the activate signal with the callback function that
    // shows the window with the statistics.
    g_signal_connect(
            view_statistics_button, "activate",
            G_CALLBACK(statistics_callback), NULL);

    // Get the statistics text view.
    statistics_text_view = 
        GTK_WIDGET(gtk_builder_get_object(builder, "statistics_text_view"));
    // Get the statistics text view's buffer.
    buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(statistics_text_view));

    // Save and load a game.
    // Get the file chooser dialog.
    file_chooser =
        GTK_WIDGET(gtk_builder_get_object(builder, "file_chooser_dialog"));

    // Get the load game button from the filechooser dialog.
    file_chooser_load_game_button =
        GTK_WIDGET(gtk_builder_get_object(
                    builder, "file_chooser_load_game_button"));
    // Connect the button-press-event signal with the callback function that
    // closes the statistics window.
    g_signal_connect(
            file_chooser_load_game_button, "button-press-event",
            G_CALLBACK(filechooser_load_game), NULL);

    // Get the cancel button from the filechooser dialog.
    file_chooser_cancel_button =
        GTK_WIDGET(gtk_builder_get_object(
                    builder, "file_chooser_cancel_button"));
    // Connect the button-press-event signal with the callback function that
    // closes the statistics window.
    g_signal_connect(
            file_chooser_cancel_button, "button-press-event",
            G_CALLBACK(filechooser_close), NULL);

    // Get the load game menubar button.
    load_game_menubar_button =
        GTK_WIDGET(gtk_builder_get_object(builder, "load_game_menubar_button"));
    // Connect the activate signal with the callback function that
    // shows the window with the statistics.
    g_signal_connect(
            load_game_menubar_button, "activate",
            G_CALLBACK(load_game_callback), NULL);

    // Get the save game menubar button.
    save_game_menubar_button =
        GTK_WIDGET(gtk_builder_get_object(builder, "save_game_menubar_button"));
    // Connect the activate signal with the callback function that
    // shows the window with the statistics.
    g_signal_connect(
            save_game_menubar_button, "activate",
            G_CALLBACK(save_game_callback), NULL);


    // Show the main menu window on application start.
    gtk_widget_show_all((GtkWidget *) main_menu_window);

    // Initialize the game state to "main_menu" and the game board.
    //initialize_board(game.board, 0, 0);
    game.state = main_menu;
    game.builder = builder;
    game.drawing_area = drawing_area;

    // Default gameplay settings.
    game.mode = single_player;
    game.turn = player_1;
    game.players_color.player_1 = white;
    game.players_color.player_2 = black;

    // Run GTK.
    gtk_main();

    // Free the memory that was dynamically allocated.
    g_slice_free(app_widgets, widgets);

    return 0;
}

// Help --> About
void on_menuitm_about_activate(GtkMenuItem *menuitem, app_widgets *app_wdgts)
{
    gtk_widget_show(app_wdgts->w_dlg_about);
}

// About dialog box Close button
void on_dlg_about_response(
        GtkDialog *dialog, gint response_id, app_widgets *app_wdgts)
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


// Called when the accept button on the main menu is activated.
void on_main_menu_ok_button_button_press_event(GtkWidget *button, Game *game)
{
    // Hide the main menu when to start the game.
    gtk_widget_hide((GtkWidget *) main_menu_window);

    // Show the main window (with the board) and its child widgets.
    gtk_widget_show_all((GtkWidget *) window);

    // Get the name of the player and store it in a variable.
    player_name =
        gtk_entry_get_text(gtk_builder_get_object(builder, "player_name"));

    // Get the name of the opponent and store it in a variable.
    opponents_name =
        gtk_entry_get_text(gtk_builder_get_object(builder, "opponents_name"));

    // Update the widgets that show the game information.
    update_game_info(game);

    // Initialize or reset the game board.
    initialize_board(game->board, 0, 0);

    // Set the game state as running.
    game->state = running;

    // Mark all squares where the first move could be made.
    mark_valid_moves(game, get_players_color(*game));

    // If the computer plays first, make the move.
    if (game->turn == player_2 && game->mode == single_player)
    {
        // Render the game board in case the CPU plays first.
        gtk_widget_queue_draw(game->drawing_area);

        Move move;
        get_machine_move(*game, &move);
        turn_transition(game, move);
    }

    // Render the game board.
    gtk_widget_queue_draw(game->drawing_area);
}


// Updates the radio buttons that set the first player.
void update_who_plays_first_buttons(
        const gchar *label_1, const gchar *label_2)
{
    // Destroy the first radio button (player 1).
    gtk_widget_destroy(player_1_first_radio);

    // Create a new radio button with the appropiate label.
    player_1_first_radio =
        gtk_radio_button_new_with_label_from_widget(
                player_2_first_radio, label_1);

    // Destroy the second radio button (player 2).
    gtk_widget_destroy(player_2_first_radio);

    player_2_first_radio =
        gtk_radio_button_new_with_label_from_widget(
                player_1_first_radio, label_2);

    // Get the GtkBox that provides the option to select the first player.
    who_plays_first_box = 
        GTK_WIDGET(gtk_builder_get_object(builder, "who_plays_first_box"));

    // Add the new button to the corresponding GtkBox container.
    gtk_box_pack_start(
            who_plays_first_box, player_1_first_radio, FALSE, TRUE, 0);
    gtk_box_pack_start(
            who_plays_first_box, player_2_first_radio, FALSE, TRUE, 0);

    // Show the new buttons.
    gtk_widget_show(player_1_first_radio);
    gtk_widget_show(player_2_first_radio);
}


// Updates the text of the select color label.
void update_select_color_label(const gchar *player)
{
    // Get the select color label.
    select_color_label = 
        GTK_WIDGET(gtk_builder_get_object(builder, "select_color_label"));

    gchar new_text[70];
    // Set the new text according to the argument.
    sprintf(new_text, "Select the color of %s's disks:", player);

    // Change the text of the select color label.
    gtk_label_set_text(select_color_label, new_text);
}


// Updates the text of the GtkLabels that go with the name inputs.
void update_enter_name_of_players_labels(
        const gchar *player_1_text, const gchar *player_2_text)
{
    // Get the player_1 label.
    player_name_label =
        GTK_WIDGET(gtk_builder_get_object(builder, "player_name_label"));

    // Get the player_2 label.
    opponents_name_label =
        GTK_WIDGET(gtk_builder_get_object(builder, "opponents_name_label"));

    gchar new_text[700];
    // Set the new text according to the argument.
    sprintf(new_text, player_1_text);

    // Change the text of the player_1 label.
    gtk_label_set_text(player_name_label, new_text);

    // Set the new text according to the argument.
    sprintf(new_text, player_2_text);

    // Change the text of the player_2 label.
    gtk_label_set_text(opponents_name_label, new_text);
}


// Callback function for when the game mode "User vs CPU" is selected.
void on_user_cpu_toggled(GtkWidget *button, Game *game)
{
    game->mode = single_player;

    // Update the radio buttons.
    update_who_plays_first_buttons("User", "CPU");

    // Update the select color label.
    update_select_color_label("user");

    // Update the labels that go with the name input.
    update_enter_name_of_players_labels(
            "Enter your name:",
            "Enter the name of the CPU:");
}


// Callback function for when the game mode "User vs user" is selected.
void on_user_user_toggled(GtkWidget *button, Game *game)
{
    game->mode = two_players;

    // Update the radio buttons.
    update_who_plays_first_buttons("User 1", "User 2");

    // Update the select color label.
    update_select_color_label("user 1");

    // Update the labels that go with the name input.
    update_enter_name_of_players_labels(
            "Enter your name:",
            "Enter the name of your opponent:");
}

// Callback function for when the game mode "CPU vs itself" is selected.
void on_cpu_itself_toggled(GtkWidget *button, Game *game)
{
    game->mode = cpu_vs_itself;

    // Update the radio buttons.
    update_who_plays_first_buttons("CPU 1", "CPU 2");

    // Update the select color label.
    update_select_color_label("CPU 1");

    // Update the labels that go with the name input.
    update_enter_name_of_players_labels(
            "Enter CPU 1's name:",
            "Enter CPU 2's name:");
}

// Callback function for when the game mode "CPU vs CPU" is selected.
void on_cpu_cpu_toggled(GtkWidget *button, Game *game)
{
    game->mode = cpu_vs_another_cpu;

    // Update the radio buttons.
    update_who_plays_first_buttons("CPU", "Opponent's CPU");

    // Update the select color label.
    update_select_color_label("CPU");

    // Update the labels that go with the name input.
    update_enter_name_of_players_labels(
            "Enter the name of your program:",
            "Enter the name of your opponent's program:");
}


// Callback function to randomly select the first player.
static void select_first_player_randomly(GtkWidget *button, Game *game)
{
    if (generate_random_bool())
        game->turn = player_1;
    else
        game->turn = player_2;
}


// Callback function to set player 1's turn as the first.
void on_player_1_first_toggled(GtkWidget *button, Game *game)
{
    game->turn = player_1;
}


// Callback function to set player 2's turn as the first.
void on_player_2_first_toggled(GtkWidget *button, Game *game)
{
    game->turn = player_2;
}


static void select_color_randomly(GtkWidget *button, Game *game)
{
    // Player 1 plays white.
    if (generate_random_bool())
    {
        game->players_color.player_1 = white;
        game->players_color.player_2 = black;
    }
    // Player 1 plays black.
    else
    {
        game->players_color.player_1 = black;
        game->players_color.player_2 = white;
    }
}

// Callback function to set player 1's disks as white.
void on_player_1_white_toggled(GtkWidget *button, Game *game)
{
    game->players_color.player_1 = white;
    game->players_color.player_2 = black;
}


// Callback function to set player 1's disks as black.
void on_player_1_black_toggled(GtkWidget *button, Game *game)
{
    game->players_color.player_1 = black;
    game->players_color.player_2 = white;
}

// Signal handler for when the user wants to start a new game from
// the game over window.
void on_game_over_start_new_game_clicked(GtkWidget *button, Game *game)
{
    // Set the game state to "main menu".
    game->state = main_menu;
    // Hide the main window before showing the main menu window.
    gtk_widget_hide((GtkWidget *) window);
    // Hide the game over window before showing main menu window.
    gtk_widget_hide((GtkWidget *) game_over_window);
    // Show the main menu window to start a new game.
    gtk_widget_show_all((GtkWidget *) main_menu_window);
}


// Callback function to be called when the "view statistics" menu
// item is selected.
static void statistics_callback(GtkWidget *button)
{
    // Load the game statistics to the GTK widget.
    load_statistics();

    // Show the statistics window with its child widgets.
    gtk_widget_show_all((GtkWidget *) statistics_window);
}


// Callback function to close the statistics window.
static void statistics_close(void)
{
    // Hide the statistics window.
    gtk_widget_hide((GtkWidget *) statistics_window);
}


static void load_game_callback(void)
{
    // Show the file chooser dialog.
    gtk_widget_show_all((GtkWidget *) file_chooser);
}


static void save_game_callback(void)
{
    return;
}

// Callback function to load the selected saved game.
static void filechooser_load_game(void)
{
    gchar *game_to_load = gtk_file_chooser_get_filename(file_chooser);

    // Open the selected file for reading.
    FILE *fp = fopen(game_to_load, "r");

    // Close the file.
    fclose(fp);
}

// Callback function to close the file chooser dialog.
static void filechooser_close(void)
{
    // Hide the filechooser dialog.
    gtk_widget_hide((GtkWidget *) file_chooser);
}


// Signal handler for when the user wants to quit the application
// from the game over window.
void on_quit_game_clicked(GtkWidget *button, Game *game)
{
    gtk_main_quit();
}
