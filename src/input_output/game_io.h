#ifndef _INPUT_OUTPUT_
#define _INPUT_OUTPUT_

#include "../game.h"

void draw_callback(GtkWidget *widget, cairo_t *cr, gpointer data);
void print_game(Game *game);
void print_invalid_input(void);
void print_illegal_move(void);
void print_no_valid_moves(void);
void prompt_user(void);
void print_game_information(Game *game, int limit, int i);
void print_select_first_player(Game *game);
void print_select_first_player_result(Game *game, char selection);
void get_game_score(
        Game *game, int i, int j, int *white_count, int *black_count);
void print_best_possible_move(Move move, int best_score);
void print_game_over(Game game);
void print_invalid_input_machine(char input_string[5]);
void print_illegal_move_machine(Move move);
void print_opponents_cpu_move(Move move);
void print_game_statistics_to_file(Game game);

#endif
