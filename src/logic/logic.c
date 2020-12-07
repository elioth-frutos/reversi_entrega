#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include "logic.h"
#include "../game.h"
#include "../main_menu/main_menu.h"
#include "../input_output/menu_io.h"
#include "../input_output/game_io.h"
#include "../minimax/minimax.h"

// Macros for aiding in the configuration of the AI matches.
#define CPU_1_READ_PATH "shared_folder/cpu_2.txt"
#define CPU_1_WRITE_PATH "shared_folder/cpu_1.txt"
#define CPU_2_READ_PATH "cpu_1.txt"
#define CPU_2_WRITE_PATH "cpu_2.txt"


void button_pressed_callback(GtkWidget *widget, GdkEvent *event, Game *game);
static void player_1_turn(Game *game, Move move);
static void player_2_turn(Game *game, Move move);
static void turn_transition(Game *game, Move move);
void mark_valid_moves(Game *game, color color);
static void mark_valid_moves_all_directions(
        Game *game, color color, direction dir);
static void mark_valid_moves_all_arrays(
        Game *game, color color, direction dir, int i, int j);
static void mark_valid_moves_array(
        Game *game, color color, direction dir,
        int i, int j, pattern previous);
color get_players_color(Game game);
static void reverse_color(Game *game, Move move, color color);
static void reverse_color_all_directions(
        Game *game, Move move, color color, direction dir);
static void reverse_color_in_array(
        Game *game, Move move, color color, direction dir,
        char going_back, int i, int j);
static void delete_all_valid_moves(Game *game, int i, int j);
static void get_human_move(Game game, Move *move);
static void get_machine_move(Game game, Move *move);
void transform_board(Game *game, Move move);
void switch_player(turn *turn);
static void get_starting_indices(direction dir, int *i, int *j);
static char get_next_array(direction dir, int *i, int* j);
static void reverse_direction(direction *dir, char *going_back);
static void change_discs_color(color *color);
static void get_next_array_position(direction dir, int *i, int *j);
static void read_user_input(Move *move, int i);
static void convert_board_to_string(
        Game *game, char string_board[], int i, int j);
char check_for_valid_moves(Game *game);
static char check_for_valid_moves_aux(Game *game, int i, int j);
static void get_opponents_cpu_move_from_file(Game *game, Move *move);
static void save_move_to_file(Move move);
static void time_delay(time_t seconds);
static void print_paso_to_file(void);


// Signal handler to be called when a "button-press-event" signal
// is detected.
void button_pressed_callback(GtkWidget *widget, GdkEvent *event, Game *game)
{
    gdouble width, height, tile_size, board_size, start_x, start_y;
    // Struct to store the move.
    Move move;

    // Get the height and width of the drawing widget.
    width = gtk_widget_get_allocated_width(widget);
    height = gtk_widget_get_allocated_height(widget);

    // Calculate the tile size.
    tile_size =
        (width < height) ?
        (width / (BOARD_SIZE + 1)) :
        (height / (BOARD_SIZE + 1));

    // Calculate the board size.
    board_size = tile_size * BOARD_SIZE;

    // Calculate the starting coordinates (for centering the board).
    start_x = (width / 2) - (board_size / 2);
    start_y = (height / 2) - (board_size / 2);

    // Get the X and Y coordinates of the mouse click event.
    gdouble x = ((GdkEventButton*) event)->x;
    gdouble y = ((GdkEventButton*) event)->y;

    // Calculate the i and j index for the board matrix.
    gint j = (x - start_x) / tile_size;
    gint i = (y - start_y) / tile_size;
    // Copy the coordinates to the "move" struct.
    move.row = i;
    move.column = j;

    if (game->state == main_menu)
    {
        game->mode = single_player;
        game->turn = player_1;
        game->players_color.player_1 = black;
        game->players_color.player_2 = white;

        initialize_board(game->board, 0, 0);
        game->state = running;

        // Mark all squares where the first move could me made.
        mark_valid_moves(game, get_players_color(*game));

        // Render the game board.
        gtk_widget_queue_draw(game->drawing_area);
    }

    // Check if the click was within range of the board, if the game
    // is running and if the status of the selected tile is "valid".
    if (
            x >= start_x && x <= start_x + board_size &&
            y >= start_y && y <= start_y + board_size &&
            game->state == running &&
            game->turn == player_1 &&
            game->board[move.row][move.column].status == valid)
    {
        // Player 1's turn.
        player_1_turn(game, move);

        if (game->state == running)
            // Player 2's turn.
            player_2_turn(game, move);
    }

    else if (game->state == game_over)
    {
        ///////////////////
        printf("GAME OVER.");
        ///////////////////
    }
    return;
}


static void player_1_turn(Game *game, Move move)
{
    // Make the move according to who's turn it is.
    switch (game->mode)
    {
        case single_player:
            // Transition to the next turn.
            turn_transition(game, move);

            break;
        case two_players:
            // Transition to the next turn.
            turn_transition(game, move);

            break;
        case cpu_vs_itself:
            get_machine_move(*game, &move);

            // Transition to the next turn.
            turn_transition(game, move);

            break;
        case cpu_vs_another_cpu:
            if (game->turn == player_1)
            {
                get_machine_move(*game, &move);
                save_move_to_file(move);
            }
            else
            {
                get_opponents_cpu_move_from_file(game, &move);
            }

            // Transition to the next turn.
            turn_transition(game, move);

            break;
    }


    // Render the game board.
    gtk_widget_queue_draw(game->drawing_area);
}


static void player_2_turn(Game *game, Move move)
{
    // Make the move according to who's turn it is.
    switch (game->mode)
    {
        case single_player:
            get_machine_move(*game, &move);

            // Transition to the next turn.
            turn_transition(game, move);
            break;
        case two_players:
            // Transition to the next turn.
            turn_transition(game, move);
            break;
        case cpu_vs_itself:
            get_machine_move(*game, &move);

            // Transition to the next turn.
            turn_transition(game, move);
            break;
        case cpu_vs_another_cpu:
            if (game->turn == player_1)
            {
                get_machine_move(*game, &move);
                save_move_to_file(move);
            }
            else
            {
                get_opponents_cpu_move_from_file(game, &move);
            }
            // Transition to the next turn.
            turn_transition(game, move);
            break;
    }

    // Render the game board.
    gtk_widget_queue_draw(game->drawing_area);
}


static void turn_transition(Game *game, Move move)
{
    // Transform board.
    transform_board(game, move);

    // If there are valid moves, switch the player.
    if (check_for_valid_moves(game))
    {
        switch_player(&game->turn);
    }
    // If there aren't valid moves, the next player
    // has to pass its turn.
    else
    {
        // If we are playing against another CPU, we have to make
        // sure to print "PASO" to the file when skipping turns
        // and to delete the file when our opponent's CPU is
        // skipping turns.
        if (game->mode == cpu_vs_another_cpu)
        {
            // Read the file (with no effect) and delete it.
            if (game->turn == player_1)
                get_opponents_cpu_move_from_file(game, &move);

            // If I don't have any valid moves to make, print "PASO".
            else
                print_paso_to_file();
        }

        // Mark all squares where the next move could be made.
        mark_valid_moves(game, get_players_color(*game));

        // If there still there aren't valid moves, the game is over.
        if (!check_for_valid_moves(game))
            game->state = game_over;

        // If there are valid moves, inform the user that a
        // turn will be skipped.
        else
            print_no_valid_moves();
    }


    ///////////////////////////
    // Render the game board.
    gtk_widget_queue_draw(game->drawing_area);
    ///////////////////////////

    return;
}


void initialize_board(Square board[BOARD_SIZE][BOARD_SIZE], int i, int j)
{
    // Base case. End of the board.
    if (i == BOARD_SIZE-1 && j == BOARD_SIZE)
        return;

    // Recursive case. End of the column. Go to next row.
    else if (j == BOARD_SIZE)
        return initialize_board(board, ++i, 0);

    // Recursive case. Go to the next column.
    if (i >= 3 && i <= 4 && j >= 3 && j <= 4)
    {
        board[i][j].status = full;
        if ((i+j) % 2 == 0)
            board[i][j].color = white;
        else
            board[i][j].color = black;
    }
    else
    {
        board[i][j].status = empty;
        board[i][j].color = white;
    }
    return initialize_board(board, i, ++j);
}


void transform_game(Game *game)
{
    Move move;

    // Base case. Game over.
    if ((*game).state == game_over)
    {
        (*game).state = main_menu;

        // Print the game board one last time before
        // exiting.
        print_game(game);

        // Annouce the result of the match.
        print_game_over(*game);

        // Print the game statistics to a file for testing
        // purposes.
        print_game_statistics_to_file(*game);

        // Exit the game.
        return;
    }

    // Recursive case. Main menu.
    else if ((*game).state == main_menu)
    {
        /////////////////////////////////////////
        main_menu_function(game);
        /////////////////////////////////////////
        //
        //(*game).mode = 4;
        //(*game).turn = player_1;
        //(*game).players_color.player_1 = black;
        //(*game).players_color.player_2 = white;
        //
        /////////////////////////////////////////

        initialize_board((*game).board, 0, 0);
        (*game).state = running;

        // Mark all squares where the first move could me made.
        mark_valid_moves(game, get_players_color(*game));
    }

    // Recursive case. Game running.
    else if ((*game).state == running)
    {
        // Print the game board.
        print_game(game);

        // Take input from the player.
        switch ((*game).mode)
        {
            case single_player:
                if ((*game).turn == player_1)
                    get_human_move(*game, &move);
                else
                    get_machine_move(*game, &move);
                break;
            case two_players:
                get_human_move(*game, &move);
                break;
            case cpu_vs_itself:
                get_machine_move(*game, &move);
                break;
            case cpu_vs_another_cpu:
                if ((*game).turn == player_1)
                {
                    get_machine_move(*game, &move);
                    save_move_to_file(move);
                }
                else
                {
                    get_opponents_cpu_move_from_file(game, &move);
                }
                break;
        }

        // Transform board.
        transform_board(game, move);

        // If there are valid moves, switch the player.
        if (check_for_valid_moves(game))
        {
            switch_player(&game->turn);
        }
        // If there aren't valid moves, the next player
        // has to pass its turn.
        else
        {
            // If we are playing against another CPU, we have to make
            // sure to print "PASO" to the file when skipping turns
            // and to delete the file when our opponent's CPU is
            // skipping turns.
            if ((*game).mode == cpu_vs_another_cpu)
            {
                // Read the file (with no effect) and delete it.
                if ((*game).turn == player_1)
                    get_opponents_cpu_move_from_file(game, &move);

                // If I don't have any valid moves to make, print "PASO".
                else
                    print_paso_to_file();
            }

            // Mark all squares where the next move could be made.
            mark_valid_moves(game, get_players_color(*game));

            // If there still there aren't valid moves, the game is over.
            if (!check_for_valid_moves(game))
                (*game).state = game_over;

            // If there are valid moves, inform the user that a
            // turn will be skipped.
            else
                print_no_valid_moves();
        }
    }
    return transform_game(game);
}


color get_players_color(Game game)
{
    if (game.turn == player_1)
        return game.players_color.player_1;
    return game.players_color.player_2;
}


void transform_board(Game *game, Move move)
{
    // Store the color of the player who will make the move.
    color color = get_players_color(*game);

    // Change the status and color of the square where the move
    // was made.
    (*game).board[move.row][move.column].status = full;
    if ((*game).turn == player_1)
        (*game).board[move.row][move.column].color = color;
    else
        (*game).board[move.row][move.column].color = color;

    // Reverse the color of the corresponding discs.
    reverse_color(game, move, color);

    // Mark all squares where the next move could me made.
    mark_valid_moves(game, !color);
}


static void reverse_color(Game *game, Move move, color color)
{
    reverse_color_all_directions(game, move, color, north);
}


static void reverse_color_all_directions(
        Game *game, Move move, color color, direction dir)
{
    // Base case. Last direction.
    if (dir == north_west)
    {
        reverse_color_in_array(game, move, color, dir, FALSE, move.row, move.column);
        return;
    }

    // Recursive case. Search on the next direction.
    reverse_color_in_array(game, move, color, dir, FALSE, move.row, move.column);
    return reverse_color_all_directions(game, move, color, ++dir);
}


static void reverse_color_in_array(
        Game *game, Move move, color color, direction dir,
        char going_back, int i, int j)
{
    // Base cases.
    if (
            // End of matrix.
            (i == BOARD_SIZE || j == BOARD_SIZE || i == -1 || j == -1) ||
            // Went back to the starting square.
            (going_back && (int)move.row == i && (int)move.column == j) ||
            // Found a non-full square.
            ((*game).board[i][j].status != full)
            )
        return;

    // Recursive case. Found a square with the same color, go back.
    if (
            !going_back &&
            (*game).board[i][j].status == full &&
            (*game).board[i][j].color == color &&
            ((int)move.row != i || (int)move.column != j))
        reverse_direction(&dir, &going_back);

    // Recursive case. Going back and reversing discs.
    else if (going_back && (*game).board[i][j].color != color)
        change_discs_color(&(*game).board[i][j].color);

    // Recursive case. Go to the next square.
    get_next_array_position(dir, &i, &j);
    return reverse_color_in_array(game, move, color, dir, going_back, i, j);
}


static void reverse_direction(direction *dir, char *going_back)
{
    *dir = (*dir + 4) % 8;
    *going_back = TRUE;
}


void mark_valid_moves(Game *game, color color)
{
    // Delete the valid moves for the previous play.
    delete_all_valid_moves(game, 0, 0);

    // Mark the valid moves for the current play.
    mark_valid_moves_all_directions(game, color, north);
}


static void mark_valid_moves_all_directions(
        Game *game, color color, direction dir)
{
    int i, j;

    // Get the starting indices for a given direction.
    get_starting_indices(dir, &i, &j);

    // Mark valid moves for all arrays in a given direction.
    mark_valid_moves_all_arrays(game, color, dir, i, j);

    // Base case. Last direction.
    if (dir == north_west)
        return;

    // Recursive case. Go to the next direction.
    return mark_valid_moves_all_directions(game, color, ++dir);
}


static void mark_valid_moves_all_arrays(
        Game *game, color color, direction dir, int i, int j)
{
    // Mark valid moves in an array.
    mark_valid_moves_array(game, color, dir, i, j, empty_pattern);

    // Base case. All arrays in a given direction were traversed.
    if (!get_next_array(dir, &i, &j))
        return;

    // Recursive case. Go to the next array.
    return mark_valid_moves_all_arrays(game, color, dir, i, j);
}


static char get_next_array(direction dir, int *i, int* j)
{
    switch (dir)
    {
        case north:
            ++(*j);
            break;
        case north_east:
            if (*i == BOARD_SIZE-1)
                ++(*j);
            else
                ++(*i);
            break;
        case east:
            ++(*i);
            break;
        case south_east:
            if (*j == 0)
                ++(*i);
            else
                --(*j);
            break;
        case south:
            --(*j);
            break;
        case south_west:
            if (*i == 0)
                --(*j);
            else
                --(*i);
            break;
        case west:
            --(*i);
            break;
        case north_west:
            if (*j == BOARD_SIZE-1)
                --(*i);
            else
                ++(*j);
            break;
    }

    // Check if we have reached the limits. Return 0 if we have.
    if (
            (*i == BOARD_SIZE || *j == BOARD_SIZE || *i == -1 || *j == -1) ||
            (dir == north_east && *j == 6) ||
            (dir == south_east && *i == 6) ||
            (dir == south_west && *j == 1) ||
            (dir == north_west && *i == 1)
            )
        return 0;

    // If we haven't reached the limits, return 1.
    return 1;
}


static void get_starting_indices(direction dir, int *i, int *j)
{
    switch (dir)
    {
        case north:
            *i = BOARD_SIZE-1;
            *j = 0;
            break;
        case north_east:
            *i = 2;
            *j = 0;
            break;
        case east:
            *i = 0;
            *j = 0;
            break;
        case south_east:
            *i = 0;
            *j = 5;
            break;
        case south:
            *i = 0;
            *j = BOARD_SIZE-1;
            break;
        case south_west:
            *i = 5;
            *j = BOARD_SIZE-1;
            break;
        case west:
            *i = BOARD_SIZE-1;
            *j = BOARD_SIZE-1;
            break;
        case north_west:
            *i = BOARD_SIZE-1;
            *j = 2;
            break;
    }
}


static void mark_valid_moves_array(
        Game *game, color color, direction dir,
        int i, int j, pattern previous)
{
    // Base case. End of matrix.
    if (i == BOARD_SIZE || j == BOARD_SIZE || i == -1 || j == -1)
        return;

    // Base case. Found a valid square.
    if (
            (*game).board[i][j].status != full &&
            previous == same_and_opposite_color)
    {
        (*game).board[i][j].status = valid;
        return;
    }

    // Recursive case.
    if ((*game).board[i][j].status != full)
        previous = empty_pattern;
    else if (
            (*game).board[i][j].status == full &&
            (*game).board[i][j].color == color)
        previous = same_color;
    else if (
            (*game).board[i][j].status == full &&
            (*game).board[i][j].color != color &&
            previous == same_color)
        previous = same_and_opposite_color;
    get_next_array_position(dir, &i, &j);
    return mark_valid_moves_array(game, color, dir, i, j, previous);
}


static void get_next_array_position(direction dir, int *i, int *j)
{
    switch (dir)
    {
        case north:
            --(*i);
            break;
        case north_east:
            --(*i);
            ++(*j);
            break;
        case east:
            ++(*j);
            break;
        case south_east:
            ++(*i);
            ++(*j);
            break;
        case south:
            ++(*i);
            break;
        case south_west:
            ++(*i);
            --(*j);
            break;
        case west:
            --(*j);
            break;
        case north_west:
            --(*i);
            --(*j);
            break;
    }
}


static void delete_all_valid_moves(Game *game, int i, int j)
{
    // Base case. End of the matrix.
    if (i == BOARD_SIZE)
        return;

    // Recursive case. Go to the next row.
    else if (j == BOARD_SIZE)
        return delete_all_valid_moves(game, ++i, 0);

    // Recursive case. Go to the next column.
    if ((*game).board[i][j].status == valid)
        (*game).board[i][j].status = empty;
    return delete_all_valid_moves(game, i, ++j);
}


void switch_player(turn *turn)
{
    if (*turn == player_1)
        *turn = player_2;
    else
        *turn = player_1;
}


static void change_discs_color(color *color)
{
    if (*color == white)
        *color = black;
    else
        *color = white;
}


static void get_human_move(Game game, Move *move)
{
    prompt_user();
    read_user_input(move, 0);

    // Base case. Valid move.
    if (game.board[move->row][move->column].status == valid)
        return;

    // Recursive case. Invalid move.
    print_illegal_move();
    return get_human_move(game, move);
}


static void read_user_input(Move *move, int i)
{
    int input = toupper(getchar());

    // Base case. Reached the end of input stream.
    if (i > 1 && (input == '\n' || input == EOF))
        return;

    // Recursive case. Store the column.
    if (i == 0)
    {
        if (input >= 'A' && input <= 'H')
        {
            move->column = input - 'A';
        }
        else
        {
            print_invalid_input();
            consume_buffer();
            prompt_user();
            return read_user_input(move, 0);
        }
    }

    // Recursive case. Store the row.
    else if (i == 1)
    {
        if (input >= '1' && input <= '8')
        {
            move->row = input - '1';
        }
        else
        {
            print_invalid_input();
            consume_buffer();
            prompt_user();
            return read_user_input(move, 0);
        }
    }
    return read_user_input(move, ++i);
}


static void get_machine_move(Game game, Move *move)
{
    //////////////////////////////////////////////////////////
    // String to store the encoded board and the player.
    //char string_board[BOARD_SIZE * BOARD_SIZE + 2];

    // String to store the BASH command to be executed.
    //char command[22 + BOARD_SIZE * BOARD_SIZE + 2] =
        //"./src/haskell/MiniMax ";

    // String to store the result of executing the Haskell
    // MiniMax module. For example: "D3", "A1", "B6", etc.
    //char move_aux[3];

    // Encode the state of the game as a string.
    //convert_board_to_string(&game, string_board, 0, 0);

    // Concatenate the strings "command" and "string_board"
    // to generate the BASH command to be executed.
    //strcat(command, string_board);

    /////////////////////////////
    //printf("\n%s\n", command);
    /////////////////////////////

    // Create a pipe and execute the Haskell MiniMax module.
    //FILE *fp = popen(command, "r");

    // Store the result in "move_aux".
    //fgets(move_aux, 3, fp);

    /////////////////////////////
    //printf("\n%s\n", move_aux);
    /////////////////////////////

    // Closes the file stream.
    //pclose(fp);
    //////////////////////////////////////////////////////////

    // Find the best move and store it in a variable.
    int best_score = find_best_move(&game, move);

    // Print the best move and the minimax score
    // associated with it.
    print_best_possible_move((*move), best_score);
}


static void convert_board_to_string(
        Game *game, char string_board[], int i, int j)
{
    // Base case. End of the board.
    if (i == BOARD_SIZE)
    {
        // Store the color of the player who will make the move
        // as the last character of the string..
        if (get_players_color(*game) == white)
            string_board[i*BOARD_SIZE + j] = 'W';
        else
            string_board[i*BOARD_SIZE + j] = 'B';
        string_board[i*BOARD_SIZE + j + 1] = '\0';
        return;
    }

    // Recursive case. End of the row.
    else if (j == BOARD_SIZE)
        return convert_board_to_string(game, string_board, ++i, 0);

    // Recursive case. Encode the current square of the board.
    if ((*game).board[i][j].status == empty)
    {
        string_board[i*BOARD_SIZE + j] = 'E';
    }
    else if ((*game).board[i][j].status == valid)
    {
        string_board[i*BOARD_SIZE + j] = 'V';
    }
    else if ((*game).board[i][j].status == full)
    {
        if ((*game).board[i][j].color == white)
            string_board[i*BOARD_SIZE + j] = 'W';
        else
            string_board[i*BOARD_SIZE + j] = 'B';
    }
    return convert_board_to_string(game, string_board, i, ++j);
}


char check_for_valid_moves(Game *game)
{
    return check_for_valid_moves_aux(game, 0, 0);
}


// Returns TRUE if there is at least one valid move.
// Returns FALSE if there are no valid moves.
static char check_for_valid_moves_aux(Game *game, int i, int j)
{
    // Base case. Reached the end of the board.
    if (i == BOARD_SIZE)
        return FALSE;

    // Recursive case. Go to the next row.
    if (j == BOARD_SIZE)
        return check_for_valid_moves_aux(game, ++i, 0);

    // Base case. We found a valid move.
    else if ((*game).board[i][j].status == valid)
        return TRUE;

    // Recursive case. Go to the next column.
    return check_for_valid_moves_aux(game, i, ++j);
}


static void get_opponents_cpu_move_from_file(Game *game, Move *move)
{
    FILE *file;
    char input_string[5];
    char input_row, input_column;

    // If the file doesn't exist yet, wait and try again.
    while (!(file = fopen(CPU_1_READ_PATH, "r")))
        // Wait 1 second.
        time_delay(1);

    // Once the file has been found, read its contents.
    fgets(input_string, 5, file);

    // Check if the opponent's CPU skipped its turn. If it did,
    // just remove the file.
    if (!strcmp(input_string, "PASO"))
    {
        remove(CPU_1_READ_PATH);
    }
    
    // If it didn't skipped its turn, read the input.
    else
    {
        input_column = input_string[0];
        input_row = input_string[1];

        // Transform the character input to indices.
        (*move).column = input_column - 'A';
        (*move).row = input_row - '1';

        // Check if the input is not valid.
        if (
                input_column < 'A' || input_column > 'H' ||
                input_row < '1' || input_row > '8')
        {
            print_invalid_input_machine(input_string);
            (*game).state = game_over;
        }

        // Check if the move is not valid.
        else if ((*game).board[move->row][move->column].status != valid)
        {
            print_illegal_move_machine(*move);
            (*game).state = game_over;
        }

        // If the move is valid, remove the file and continue playing.
        else
        {
            remove(CPU_1_READ_PATH);
            print_opponents_cpu_move(*move);
        }
    }

    // Close the file.
    fclose(file);
}


static void save_move_to_file(Move move)
{
    // Create a new file.
    FILE *fp = fopen(CPU_1_WRITE_PATH, "w");

    // Save the move.
    fputc(move.column + 'A', fp);
    fputc(move.row + '1', fp);

    // Close the file.
    fclose(fp);
}


static void print_paso_to_file(void)
{
    // Create a new file.
    FILE *fp = fopen(CPU_1_WRITE_PATH, "w");

    // Write "PASO" to the file to indicate that
    // a turn will be skipped.
    fputs("PASO", fp);

    // Close the file.
    fclose(fp);
}


// Creates a time delay.
static void time_delay(time_t seconds)
{
    // Declare and initialize a new timespec struct.
    struct timespec time;
    time.tv_sec = seconds;
    time.tv_nsec = 0;

    // Sleep for the required amount of time.
    nanosleep(&time, NULL);
}
