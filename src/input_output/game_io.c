#include <stdio.h>
#include "../game.h"

void print_game_information(Game *game);
static void print_horizontal_separators(int j);
static void print_horizontal_indices(int j);
static void print_game_aux(Game *game, int i, int j);
static void print_game_mode(Game *game);
static void print_game_score_and_color(Game *game);
void get_game_score(
        Game *game, int i, int j, int *white_count, int *black_count);
static void print_header_separator(int i);


void print_game(Game *game)
{
    print_game_information(game);
    print_game_aux(game, 0, 0);
}


void print_game_information(Game *game)
{
    // Print the big header separator.
    print_header_separator(0);

    // Print the game mode.
    print_game_mode(game);

    // Print score of each player with their
    // corresponding colors.
    print_game_score_and_color(game);
}


// Prints the game board.
//
// Arguments:
// The "game" struct.
// Two counters, "i" and "j" (the functions is recursive).
static void print_game_aux(Game *game, int i, int j)
{
    // Base case. End of the board.
    if (i == BOARD_SIZE)
    {
        print_horizontal_separators(0);
        // Print the horizontal indices (alphabetic).
        print_horizontal_indices(0);
        return;
    }

    // Recursive case. End of the row. Go to the next column.
    else if (j == BOARD_SIZE)
    {
        //printf("|\n");
        // Print the vertical indices (numerical).
        printf("| %c \n", '1' + i);
        return print_game_aux(game, ++i, 0);
    }

    if (j == 0)
    {
        // If it's the very first call, print the horizontal
        // indices (alphabetic).
        if (i == 0)
        {
            printf("\n");
            print_horizontal_indices(0);
        }
        // Print the horizontal separators.
        print_horizontal_separators(0);
        // Print the vertical indices (numerical).
        printf(" %c ", '1' + i);
    }

    // Recursive case. Print something.
    if ((*game).board[i][j].status == full)
    {
        if ((*game).board[i][j].color == white)
            printf("| W ");
        else if ((*game).board[i][j].color == black)
            printf("| B ");
    }
    else if ((*game).board[i][j].status == valid)
    {
        printf("| - ");
    }
    else if ((*game).board[i][j].status == empty)
    {
        printf("|   ");
    }

    return print_game_aux(game, i, ++j);
}


void print_invalid_input(void)
{
    printf("\nInvalid input.\n");
}


void print_illegal_move(void)
{
    printf("\nThat move is not legal.\n");
}


void prompt_user(void)
{
    printf("\nEnter the column and the row of the next play: ");
}


void print_no_valid_moves(void)
{
    printf("\nThere are no moves. Skipping turn.\n");
}


void print_best_possible_move(Move move, int best_score)
{
    printf(
            "\nThe best possible move is %c%c with a minimax score "
            "of %d.\n", move.column + 'A', move.row + '1', best_score);
}


// Print the horizontal separators for every row
// in the board.
//
// Arguments:
// A counter, j, because the function is recursive.
static void print_horizontal_separators(int j)
{
    // Base case.
    if (j == BOARD_SIZE-1)
    {
        printf("+---+\n");
        return;
    }

    // Recursive cases.
    if (j == 0)
        printf("   +---");
    else
        printf("+---");

    return print_horizontal_separators(++j);
}


static void print_horizontal_indices(int j)
{
    // Base case. End of the row. Print the new line.
    if (j == BOARD_SIZE)
    {
        printf("\n");
        return;
    }

    // Recursive cases. Print the index.
    if (j == 0)
        printf("   ");

    printf("  %c ", 'A' + j);
    return print_horizontal_indices(++j);
}


static void print_margin(void)
{
    printf("            ");
}


static void print_game_score_and_color(Game *game)
{
    int black_count = 0;
    int white_count = 0;

    // Calculate the game score.
    get_game_score(game, 0, 0, &white_count, &black_count);

    // Print the current game score along with the
    // corresponding color of each player.
    switch ((*game).mode)
    {
        case single_player:
            if ((*game).players_color.player_1 == black)
            {
                printf("User (black): %d | ", black_count);
                printf("CPU (white): %d", white_count);
            }
            else
            {
                printf("User (white): %d | ", white_count);
                printf("CPU (black): %d", black_count);
            }
            break;
        case two_players:
            if ((*game).players_color.player_1 == black)
            {
                printf("User (black): %d | ", black_count);
                printf("Player 2 (white): %d", white_count);
            }
            else
            {
                printf("User (white): %d | ", white_count);
                printf("Player 2 (black): %d", black_count);
            }
            break;
        case cpu_vs_itself:
            if ((*game).players_color.player_1 == black)
            {
                printf("CPU 1 (black): %d | ", black_count);
                printf("CPU 2 (white): %d", white_count);
            }
            else
            {
                printf("CPU 1 (white): %d | ", white_count);
                printf("CPU 2 (black): %d", black_count);
            }
            break;
        case cpu_vs_another_cpu:
            if ((*game).players_color.player_1 == black)
            {
                printf("CPU (black): %d | ", black_count);
                printf("Opponent's CPU (white): %d", white_count);
            }
            else
            {
                printf("CPU (white): %d | ", white_count);
                printf("Opponent's CPU (black): %d", black_count);
            }
            break;
    }
    printf("\n");
}


void get_game_score(
        Game *game, int i, int j, int *white_count, int *black_count)
{
    // Base case. End of the board.
    if (i == BOARD_SIZE)
        return;

    // Recursive case. End of the row. Go to the next row.
    if (j == BOARD_SIZE)
        return get_game_score(game, ++i, 0, white_count, black_count);

    // Recursive case. Go to the next column.
    if ((*game).board[i][j].status == full)
    {
        if ((*game).board[i][j].color == white)
            ++(*white_count);
        else
            ++(*black_count);
    }
    return get_game_score(game, i, ++j, white_count, black_count);
}


static void print_game_mode(Game *game)
{
    printf("Game mode: ");
    switch ((*game).mode)
    {
        case single_player:
            printf("single player\n");
            break;
        case two_players:
            printf("two players\n");
            break;
        case cpu_vs_itself:
            printf("CPU 1 vs CPU 2\n");
            break;
        case cpu_vs_another_cpu:
            printf("CPU vs opponent's CPU\n");
            break;
    }
    printf("\n");
}


static void print_header_separator(int i)
{
    // Base case.
    if (i == 3)
    {
        printf("\n\n");
        return;
    }

    // Recursive case.
    if (i == 0)
        printf("\n\n");
    printf("=========================");
    return print_header_separator(++i);
}


void print_game_over(Game game)
{
    // Print the game score and color one last time.
    printf("\n");
    print_game_score_and_color(&game);
    printf("\n");

    int black_count = 0;
    int white_count = 0;

    // Count the amount of black and white discs.
    get_game_score(&game, 0, 0, &white_count, &black_count);

    // Draw. Exit the function.
    if (black_count == white_count)
    {
        printf("\nThe game ended in a draw!\n");
        return;
    }

    // If the game didn't end in a draw, check which player won.
    color winner = (black_count > white_count) ? black : white;

    // Announce the winner.
    switch (game.mode)
    {
        case single_player:
            if (game.players_color.player_1 == winner)
                printf("\nYou win. Congratulations!\n");
            else
                printf("\nGame over. The CPU wins!\n");
            break;
        case two_players:
            if (game.players_color.player_1 == winner)
                printf("\nYou win. Congratulations!\n");
            else
                printf("\nGame over. Your opponent wins!\n");
            break;
        case cpu_vs_itself:
            if (game.players_color.player_1 == winner)
                printf("\nThe CPU 1 wins!\n");
            else
                printf("\nThe CPU 2 wins!\n");
            break;
        case cpu_vs_another_cpu:
            if (game.players_color.player_1 == winner)
                printf("\nThe CPU wins!\n");
            else
                printf("\nThe opponent's CPU wins!\n");
            break;
    }
}


void print_invalid_input_machine(char input_string[5])
{
    printf(
            "\nThe input read from the file (%s) was "
            "invalid.\n", input_string);
}


void print_illegal_move_machine(Move move)
{
    printf(
            "\nThe move read from the file (%c%c) was "
            "illegal.\n", move.column + 'A', move.row + '1');
}


void print_opponents_cpu_move(Move move)
{
    printf(
            "\nThe opponent's CPU decided to play on position"
            " %c%c.\n", move.column + 'A', move.row + '1');
}


// Prints the results of every match in a text file.
void print_game_statistics_to_file(Game game)
{
    int black_count;
    int white_count;
    char output_string[42];

    // Count the amount of black and white discs.
    get_game_score(&game, 0, 0, &white_count, &black_count);

    // Create a new file.
    FILE *fp = fopen("statistics.txt", "a");

    // Create the formatted string to be printed to the file.
    sprintf(
            output_string,
            "White: %d | Black: %d",
            white_count, black_count);

    fputs(output_string, fp);
    fputc('\n', fp);

    // Close the file.
    fclose(fp);

}
