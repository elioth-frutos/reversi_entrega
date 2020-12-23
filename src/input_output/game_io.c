#include <stdio.h>
#include <gtk/gtk.h>
#include <cairo.h>
#include "../game.h"
#include "../logic/logic.h"

void draw_callback(GtkWidget *widget, cairo_t *cr, Game *game);
static void draw_discs(
        cairo_t *cr, gdouble tile_size, gdouble start_x, gdouble start_y,
        Game *game);
void print_game_information(Game *game);
static void print_horizontal_separators(int j);
static void print_horizontal_indices(int j);
static void print_game_aux(Game *game, int i, int j);
static void print_game_mode(Game *game);
static void print_game_score_and_color(Game *game);
void get_game_score(
        Game *game, int i, int j, int *white_count, int *black_count);
static void print_header_separator(int i);
static void store_string(gint i, gchar *source, gchar *dest);
static int copy_entire_file_except_line(
        gint line_number, gint played_int, gint won_int, gint lost_int,
        gint draws_int);
static int is_player_on_statistics(void);



// Transforms a value in the range of [0, 255] to [0, 1].
// Used for RGB color coding.
//
// Arguments:
// - A value in the range [0, 255].
//
// Return value:
// - A value in the range [0, 1].
static float color_code(float color)
{
    return color / 255;
}

// Signal handler to be called when a "draw" signal
// is detected.
void draw_callback(GtkWidget *widget, cairo_t *cr, Game *game)
{
    gdouble width, height, tile_size, board_size, start_x, start_y;

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

    // Render the background color.
    cairo_set_source_rgba(
            cr, color_code(102), color_code(152), color_code(227), 0.3);
    // Create a path that forms a rectangle (for the background).
    cairo_rectangle(cr, 0, 0, width, height);
    // Fill the path.
    cairo_fill(cr);

    // Render each tile of the board.
    for (int i = 0; i < BOARD_SIZE; i++)
    {
        for (int j = 0; j < BOARD_SIZE; j++)
        {
            // Set the x and y coordinates for each square.
            int x = start_x + j * tile_size;
            int y = start_y + i * tile_size;

            // Set the color code depending on the parity of
            // the sum of the square's indeces.
            if ((i + j) % 2 == 0)
                cairo_set_source_rgba(
                        cr,
                        color_code(102),
                        color_code(152),
                        color_code(227),
                        1);
            else
                cairo_set_source_rgba(
                        cr,
                        color_code(26),
                        color_code(84),
                        color_code(171),
                        1);
           
            // Create a path that forms a rectangle (for the tile).
            cairo_rectangle(cr, x, y, tile_size, tile_size);
            // Fill the path.
            cairo_fill(cr);
        }
    }

    // Render the gaming board's border.
    // Increase the path's width.
    cairo_set_line_width (cr, 5);
    // Create a path that forms a rectangle (around the board).
    cairo_rectangle(cr, start_x, start_y, board_size, board_size);
    // Draw the path.
    cairo_stroke(cr);

    // Draw the discs.
    draw_discs(cr, tile_size, start_x, start_y, game);

    return;
}


// Draws the discs.
static void draw_discs(
        cairo_t *cr, gdouble tile_size, gdouble start_x, gdouble start_y,
        Game *game)
{
    // Calculate the radius of the discs.
    gdouble radius = (tile_size / 2) * 0.7;
    gdouble radius_2 = (tile_size / 2) * 0.5;

    // Traverse the matrix and draw the discs.
    for (int i = 0; i < BOARD_SIZE; i++)
    {
        for (int j = 0; j < BOARD_SIZE; j++)
        {
            // X and Y position of every disc on the board.
            int x = start_x + j * tile_size + (tile_size / 2);
            int y = start_y + i * tile_size + (tile_size / 2);

            // Check the color of the discs and draw accordingly.
            if (game->board[i][j].status == full)
            {
                // White disc.
                if (game->board[i][j].color == white)
                {
                    // Set the color.
                    cairo_set_source_rgba(
                            cr, color_code(255), color_code(255),
                            color_code(255), 1);
                    // Create a path that forms a circle.
                    cairo_arc(cr, x, y, radius, 0, 2*G_PI);
                    // Fill the path.
                    cairo_fill(cr);

                    // Draw the inner circle.
                    cairo_set_source_rgba(
                            cr, color_code(200), color_code(200),
                            color_code(200), 1);
                    cairo_arc(cr, x, y, radius_2, 0, 2*G_PI);
                    cairo_fill(cr);
                }
                // Black disc.
                else if (game->board[i][j].color == black)
                {
                    // Set the color.
                    cairo_set_source_rgba(
                            cr, color_code(0), color_code(0),
                            color_code(0), 1);
                    // Create a path that forms a circle.
                    cairo_arc(cr, x, y, radius, 0, 2*G_PI);
                    // Fill the path.
                    cairo_fill(cr);

                    // Draw the inner circle.
                    cairo_set_source_rgba(
                            cr, color_code(100), color_code(100),
                            color_code(100), 1);
                    cairo_arc(cr, x, y, radius_2, 0, 2*G_PI);
                    cairo_fill(cr);
                }
            }

            // Draw the "valid" mark.
            else if (
                    game->board[i][j].status == valid &&
                    game->turn == player_1)
            {
                // Set the color.
                cairo_set_source_rgba(
                        cr, color_code(255), color_code(0), color_code(0), 1);
                // Set the path's width.
                cairo_set_line_width (cr, 2);
                // Create a path that forms a circle.
                cairo_arc(cr, x, y, radius, 0, 2*G_PI);
                // Create a stroke along the recently created path.
                cairo_stroke(cr);
            }
        }
    }
}


// Updates the GtkLabels that show the game information and status.
void update_game_info(Game *game)
{
    gchar new_text[700];
    int black_count = 0, white_count = 0,
        player_1_count = 0, player_2_count = 0;
    gchar player_1_color[10], player_2_color[10];

    // Count the amount of black and white discs.
    get_game_score(game, 0, 0, &white_count, &black_count);

    // Store the counts according to the players' colors.
    if (game->players_color.player_1 == white)
    {
        player_1_count = white_count;
        player_2_count = black_count;
        strcpy(player_1_color, "white");
        strcpy(player_2_color, "black");
    }
    else
    {
        player_2_count = white_count;
        player_1_count = black_count;
        strcpy(player_1_color, "black");
        strcpy(player_2_color, "white");
    }

    // Set the new text according to who's turn it is.
    if (game->turn == player_1)
        sprintf(new_text, "It's %s's turn.", player_name);
    else
        sprintf(new_text, "It's %s's turn.", opponents_name);

    // Change the text of the label that shows the turn.
    gtk_label_set_text(turn_info_label, new_text);

    // Set the text that will show the current score of both players.
    sprintf(
            new_text,
            "%s (%s): %d | %s (%s): %d",
            player_name, player_1_color, player_1_count,
            opponents_name, player_2_color, player_2_count);

    // Change the text of the label that shows the scores.
    gtk_label_set_text(score_info_label, new_text);
}


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


// Update the game's database to include the last match.
void update_game_statistics(Game game)
{
    int black_count = 0;
    int white_count = 0;
    int line_number;
    gchar output_string[1000];

    // Count the amount of black and white discs.
    get_game_score(&game, 0, 0, &white_count, &black_count);

    // Update the flags according to the results of the game.
    int games_won = 0, games_lost = 0, draws = 0;
    if (black_count > white_count)
    {
        if (game.players_color.player_1 == black)
            games_won++;
        else
            games_lost++;
    }
    else if (black_count < white_count)
    {
        if (game.players_color.player_1 == black)
            games_lost++;
        else
            games_won++;
    }
    else
    {
        draws++;
    }

    // If the player's name is already on the file.
    if ((line_number = is_player_on_statistics()) != -1)
    {
        gchar line[1000];
        gchar played_str[20], won_str[20], lost_str[20], draws_str[10];
        gint played_int, won_int, lost_int, draws_int;

        // Open the statistics file for reading and writing.
        FILE *fp = fopen("statistics.txt", "r+");

        // Go to the line "line_number".
        for(int i = line_number; i >= 0; i--)
            // Consume all the characters in the line.
            while(fgetc(fp) != '\n');

        // Store the relevant line on the "line" variable.
        fgets(line, 1000, fp);
        // Store the relevant numbers as strings.
        for(int i = 0; i < 1000; i++)
        {
            // Games played.
            if (line[i-2] == ':' && line[i-3] == 'd')
                store_string(i, line, played_str);

            // Games won.
            else if (line[i-2] == ':' && line[i-3] == 'n')
                store_string(i, line, won_str);

            // Games lost.
            else if (line[i-2] == ':' && line[i-3] == 't')
                store_string(i, line, lost_str);

            // Games that ended in a draw.
            else if (line[i-2] == ':' && line[i-3] == 's')
                store_string(i, line, draws_str);
        }

        // Convert the strings to integers.
        played_int = atoi(played_str);
        won_int = atoi(won_str);
        lost_int = atoi(lost_str);
        draws_int = atoi(draws_str);

        // Update the variables with the results of the latest game.
        played_int++;
        won_int += games_won;
        lost_int += games_lost;
        draws_int += draws;

        // Close the file.
        fclose(fp);

        // Copy the entire file except the line "line_number", which
        // will be replaced with the updated data.
        copy_entire_file_except_line(
                line_number, played_int, won_int, lost_int, draws_int);

    }
    // If the player's name is not on the file.
    else
    {
        // Open the statistics file for reading and writing.
        FILE *fp = fopen("statistics.txt", "a");

        // Create the formatted string to be printed to the file.
        sprintf(
                output_string,
                "-> %s\n| Games played: %d | Games won: %d | Games lost: %d |"
                " Draws: %d |\n",
                 player_name, 1, games_won, games_lost, draws);

        fputs(output_string, fp);
        fputc('\n', fp);

        // Close the file.
        fclose(fp);
    }
}


static void store_string(gint i, gchar *source, gchar *dest)
{
    int j = i;
    for(j = i; source[j] != ' '; j++)
        dest[j-i] = source[j];
    dest[j-i] = '\0';
}


static int copy_entire_file_except_line(
        gint line_number, gint played_int, gint won_int, gint lost_int,
        gint draws_int)
{
    gchar aux[1000];
    gchar output_string[1000];

    // Open the original file.
    FILE *fp = fopen("statistics.txt", "r");
    // Open an auxiliary file.
    FILE *fp_2 = fopen("statistics_aux", "w");

    // Copy the contents of file_1 to file_2 up to the line "line_number".
    for(int i = 0; i < line_number && fgets(aux, 1000, fp); i++)
        fputs(aux, fp_2);

    // Create the formatted string with the updated data.
    sprintf(
            output_string,
            "-> %s\n| Games played: %d | Games won: %d | Games lost: %d |"
            " Draws: %d |\n",
             player_name, played_int, won_int, lost_int, draws_int);

    // Store the updated data into the file.
    fputs(output_string, fp_2);

    // Do not copy the next 2 lines (old data).
    for(int i = 0; i < 2; i++)
        fgets(aux, 1000, fp);

    // Copy the rest of the contents of file_1 to file_2.
    while(fgets(aux, 1000, fp))
        fputs(aux, fp_2);

    // Close the files.
    fclose(fp_2);
    fclose(fp);

    // Remove the old file.
    remove("statistics.txt");
    // Rename the new file.
    rename("statistics_aux", "statistics.txt");
}


static int is_player_on_statistics(void)
{
    gchar aux[1000];
    gchar name[1000];
    int line_number = 0;

    // Open the statistics file for reading and writing.
    FILE *fp = fopen("statistics.txt", "r");

    // Scan every line of the file.
    while(fgets(aux, 1000, fp))
    {
        int i = 3;
        // Check if we have found a line with a name.
        if (strlen(aux) > 4 && aux[0] == '-' && aux[1] == '>' && aux[2] == ' ')
        {
            for(i = 3; i < 1002 && aux[i] != '\n'; i++)
            {
                name[i-3] = aux[i];
            }
            name[i-3] = '\0';
        }
        // Check if the player name matches with the one in the file.
        if (strcmp(name, player_name) == 0)
        {
            // Close the file.
            fclose(fp);

            // A match was found. Return the line number.
            return line_number;
        }
        line_number++;
    }

    // Close the file.
    fclose(fp);

    // No match was found in the file.
    return -1;
}
