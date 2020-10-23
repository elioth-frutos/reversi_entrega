#include "../game.h"
#include "../input_output/menu_io.h"
#include "../input_output/game_io.h"

static void select_game_mode(Game *game);
static void select_first_player(Game *game);
static void select_discs_color(Game *game);


void main_menu_function(Game *game)
{
    select_game_mode(game);
    select_first_player(game);
    select_discs_color(game);
}


static void select_game_mode(Game *game)
{
    // Print the game mode menu.
    print_select_game_mode();

    // Store the user's selection.
    char selection = validate_integer(1, 4);
    (*game).mode = selection;

    // Print the result of the user's selection.
    print_select_game_mode_result(selection);
}


static void select_first_player(Game *game)
{
    int selection;

    // Print the menu to select the first player.
    print_select_first_player(game);

    // Check if the user chose to select the
    // first player randomly.
    if ((selection = validate_integer(1, 3)) == 3)
        selection = generate_random_bool() + 1;

    // Print the selected option.
    print_select_first_player_result(game, selection);

    // Set the turn to the corresponding player.
    if (selection == 1)
        (*game).turn = player_1;
    else
        (*game).turn = player_2;

}


static void select_discs_color(Game *game)
{
    int random_int = generate_random_bool();

    // Print the menu to select the color of the discs.
    print_select_discs_color(game);

    // Store the selection of the user.
    switch (validate_integer(1, 3))
    {
        case 1:
            (*game).players_color.player_1 = black;
            (*game).players_color.player_2 = white;
            break;
        case 2:
            (*game).players_color.player_1 = white;
            (*game).players_color.player_2 = black;
            break;
        case 3:
            (*game).players_color.player_1 = random_int;
            (*game).players_color.player_2 = !random_int;
            break;
    }

    // Print the result of the selected option.
    print_select_discs_color_result(game);
}
