#ifndef _GAME_
#define _GAME_

#define FALSE 0
#define TRUE 1
#define BOARD_SIZE 8

typedef enum square_status
{
    empty = 0,
    valid = 1,
    full = 2
} square_status;

typedef enum color
{
    white = 0,
    black = 1
} color;

typedef struct Square
{
    square_status status;
    color color;
} Square;

typedef enum turn
{
    player_1 = 1,
    player_2 = 2
} turn;

typedef struct Players_color
{
    color player_1;
    color player_2;
} Players_color;

typedef enum column
{
    A = 0,
    B = 1,
    C = 2,
    D = 3,
    E = 4,
    F = 5,
    G = 6,
    H = 7
} column;

typedef enum row
{
    r1 = 0,
    r2 = 1,
    r3 = 2,
    r4 = 3,
    r5 = 4,
    r6 = 5,
    r7 = 6,
    r8 = 7
} row;

typedef struct Move
{
    column column;
    row row;
} Move;

typedef enum game_mode
{
    single_player = 1,
    two_players = 2,
    cpu_vs_itself = 3,
    cpu_vs_another_cpu = 4
} game_mode;

typedef enum game_state
{
    main_menu = 0,
    running = 1,
    game_over = 2
} game_state;

typedef struct Game
{
    Square board[BOARD_SIZE][BOARD_SIZE];
    game_mode mode;
    game_state state;
    turn turn;
    Players_color players_color;
} Game;

#endif
