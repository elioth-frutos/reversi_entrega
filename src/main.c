#include "game.h"
#include "logic/logic.h"
#include "input_output/menu_io.h"


int main(void)
{
    // Declare a game struct to store
    // the state of the game.
    Game game;

    // Print the welcome message.
    print_welcome_message();

    // Initialize the game state to main_menu.
    game.state = main_menu;
    
    // Transform the game until the user
    // decides to quit.
    transform_game(&game);

    return 0;
}
