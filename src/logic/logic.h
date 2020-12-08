#ifndef LOGIC
#define LOGIC

#include "../game.h"


typedef enum direction
{
    north = 0,
    north_east = 1,
    east = 2,
    south_east = 3,
    south = 4,
    south_west = 5,
    west = 6,
    north_west = 7
} direction;

typedef enum pattern
{
    empty_pattern = 0,
    same_color = 1,
    same_and_opposite_color = 2
} pattern;


void button_pressed_callback(GtkWidget *widget, GdkEvent *event, Game *game);
void start_game(Game *game);
void initialize_board(Square board[BOARD_SIZE][BOARD_SIZE], int i, int j);
void transform_game(Game *game);
char check_for_valid_moves(Game *game);
color get_players_color(Game game);
void transform_board(Game *game, Move move);
void mark_valid_moves(Game *game, color color);
void switch_player(turn *turn);

#endif
