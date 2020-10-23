#include "../input_output/game_io.h"
#include "../logic/logic.h"

// Minimizer has won.
#define MIN_SCORE -10000

// Maximizer has won.
#define MAX_SCORE 10000

// Maximum recursion depth.
#define MAX_DEPTH 7

// Huge number used as a starting value when
// finding the maximum and minimum scores.
#define HUGE_NUMBER 1000000

// Score associated with controlling a corner of the
// board (should always be greater than MAX_DEPTH).
#define CORNER_VALUE 30

static int min(int a, int b);
static int max(int a, int b);
static int minimax(
        Game *game, int depth, char is_max, int alpha, int beta);
static int evaluate(Game *game);
static int evaluate_corners(Game *game);


// Finds the best move possible in this board configuration
// using the Minimax algorithm.
//
// Arguments:
// The game struct, which represents the state of the game.
// A "move" struct to store the best possible move.
//
// Returns the minimax score of the best possible move,
// being black the minimizer and white the maximizer.
int find_best_move(Game *game, Move *move)
{
    char is_max;
    int best_score;

    // Auxiliary variable to store the tentative moves on
    // each node.
    Move aux_move;

    // Find out if the player is the minimizer or the
    // maximizer.
    if (get_players_color(*game) == black)
        is_max = 0;
    else
        is_max = 1;

    // Save the state of the game before trying a move.
    Game game_copy = (*game);

    // Try every valid move possible and save the one
    // that generates the best outcome of the player.
    // Minimizer's turn (black discs).
    if (!is_max)
    {
        best_score = HUGE_NUMBER;
        for (int i = 0; i < BOARD_SIZE; i++)
        {
            for (int j = 0; j < BOARD_SIZE; j++)
            {
                if ((*game).board[i][j].status == valid)
                {
                    // Save the move.
                    aux_move.column = j;
                    aux_move.row = i;

                    // Make the move and transform the board.
                    transform_board(game, aux_move);

                    // If there are valid moves, switch the player.
                    if (check_for_valid_moves(game))
                    {
                        switch_player(&game->turn);
                        is_max = !is_max;
                    }

                    // If there aren't valid moves, the next player
                    // has to pass its turn and we don't switch players.
                    else
                    {
                        // Mark all squares where the next move could be made.
                        mark_valid_moves(game, get_players_color(*game));
                    }

                    // Calculate the score for this move.
                    int move_score =
                        minimax(game, 0, is_max, -HUGE_NUMBER, HUGE_NUMBER);

                    // If this move produces a better (lower) score
                    // than the current best score, this is the new
                    // best move.
                    if (best_score > move_score)
                    {
                        (*move).row = i;
                        (*move).column = j;
                        best_score = move_score;
                    }

                    // Undo the move.
                    (*game) = game_copy;
                }
            }
        }
    }

    // Maximizer's turn (white discs).
    else
    {
        best_score = -HUGE_NUMBER;
        for (int i = 0; i < BOARD_SIZE; i++)
        {
            for (int j = 0; j < BOARD_SIZE; j++)
            {
                if ((*game).board[i][j].status == valid)
                {
                    // Save the move.
                    aux_move.column = j;
                    aux_move.row = i;

                    // Make the move and transform the board.
                    transform_board(game, aux_move);

                    // If there are valid moves, switch the player.
                    if (check_for_valid_moves(game))
                    {
                        // Switch the player.
                        switch_player(&game->turn);
                        is_max = !is_max;
                    }

                    // If there aren't valid moves, the next player
                    // has to pass its turn.
                    else
                    {
                        // Mark all squares where the next move could be made.
                        mark_valid_moves(game, get_players_color(*game));
                    }

                    // Calculate the score for this move.
                    int move_score =
                        minimax(game, 0, is_max, -HUGE_NUMBER, HUGE_NUMBER);

                    // If this move produces a better (higher) score
                    // than the current best score, this is the new
                    // best move.
                    if (best_score < move_score)
                    {
                        (*move).row = i;
                        (*move).column = j;
                        best_score = move_score;
                    }

                    // Undo the move.
                    (*game) = game_copy;
                }
            }
        }
    }

    // Return the best possible score.
    return best_score;
}


// Evaluation function. Black is the minimizer and white is the
// maximizer.
static int evaluate(Game *game)
{
    // If there are no move valid moves, check who won.
    //
    // The minimax() and find_best_move() function take care of
    // passing a turn when appropiate. If here we don't have
    // valid moves, it means that the game is over.
    if (!check_for_valid_moves(game))
    {
        int black_count;
        int white_count;

        // Count the amount of black and white discs.
        get_game_score(game, 0, 0, &white_count, &black_count);

        // Minimizer (black) has won.
        if (black_count > white_count)
            return MIN_SCORE;

        // Maximizer (white) has won.
        else if (black_count < white_count)
            return MAX_SCORE;

        // Draw.
        else
            return 0;
    }

    // If there are valid moves, the game isn't over.
    return 0;
}


static int minimax(
        Game *game, int depth, char is_max, int alpha, int beta)
{
    // Boolean value to know if the child of this move
    // will be pruned.
    int prune = FALSE;

    // Auxiliary variable to store the tentative moves on
    // each node.
    Move aux_move;

    // Variable for storing the score of the best possible
    // move on this level.
    int best_score;

    // Calculate the score for the current board configuration.
    int score = evaluate(game);

    // Base cases.
    //
    // The minimizing player won.
    if (score == MIN_SCORE)
        return score + depth + evaluate_corners(game);

    // The maximizing player won.
    else if (score == MAX_SCORE)
        return score - depth + evaluate_corners(game);

    // Maximum depth has been reached and nobody won.
    else if (depth == MAX_DEPTH)
        return score + evaluate_corners(game);

    // Save the state of the game before trying a move.
    Game game_copy = (*game);

    // Recursive case. Try every valid move possible.
    // Minimizer's turn (black discs).
    if (!is_max)
    {
        best_score = HUGE_NUMBER;
        for (int i = 0; i < BOARD_SIZE; i++)
        {
            // If the child of this node will be pruned,
            // get out of the loop.
            if (prune)
                break;

            // Else continue with the evaluation.
            for (int j = 0; j < BOARD_SIZE; j++)
            {
                if ((*game).board[i][j].status == valid)
                {
                    // Save the move.
                    aux_move.column = j;
                    aux_move.row = i;

                    // Make the move and transform the board.
                    transform_board(game, aux_move);

                    // If there are valid moves, switch the player.
                    if (check_for_valid_moves(game))
                    {
                        switch_player(&game->turn);
                        is_max = !is_max;
                    }

                    // If there aren't valid moves, the next player
                    // has to pass its turn and we don't switch players.
                    else
                    {
                        // Mark all squares where the next move could be made.
                        mark_valid_moves(game, get_players_color(*game));
                    }

                    // Calculate the score for this move.
                    int move_score =
                        minimax(game, depth+1, is_max, alpha, beta);

                    // If this move produces a better (lower) score
                    // than the current best score, this is the new
                    // best move.
                    best_score = min(best_score, move_score);
                    beta = min(beta, move_score);
                    if (beta <= alpha)
                        prune = TRUE;

                    // Undo the move.
                    (*game) = game_copy;
                }
            }
        }
    }

    // Maximizer's turn (white discs).
    else
    {
        best_score = -HUGE_NUMBER;
        for (int i = 0; i < BOARD_SIZE; i++)
        {
            // If the child of this node will be pruned,
            // get out of the loop.
            if (prune)
                break;

            // Else continue with the evaluation.
            for (int j = 0; j < BOARD_SIZE; j++)
            {
                if ((*game).board[i][j].status == valid)
                {
                    // Save the move.
                    aux_move.column = j;
                    aux_move.row = i;

                    // Make the move and transform the board.
                    transform_board(game, aux_move);

                    // If there are valid moves, switch the player.
                    if (check_for_valid_moves(game))
                    {
                        switch_player(&game->turn);
                        is_max = !is_max;
                    }

                    // If there aren't valid moves, the next player
                    // has to pass its turn and we don't switch players.
                    else
                    {
                        // Mark all squares where the next move could be made.
                        mark_valid_moves(game, get_players_color(*game));
                    }

                    // Calculate the score for this move.
                    int move_score =
                        minimax(game, depth+1, is_max, alpha, beta);

                    // If this move produces a better (higher) score
                    // than the current best score, this is the new
                    // best move.
                    best_score = max(best_score, move_score);
                    alpha = max(alpha, move_score);
                    if (beta <= alpha)
                        prune = TRUE;

                    // Undo the move.
                    (*game) = game_copy;
                }
            }
        }
    }

    // Return the best score. Because in minimax we assume that
    // each player is playing perfectly.
    return best_score;
}


// Get the minimum value between two integers.
static int min(int a, int b)
{
    if (a < b)
        return a;
    else
        return b;
}


// Get the maximum value between two integers.
static int max(int a, int b)
{
    if (a > b)
        return a;
    else
        return b;
}


// Returns a score associated with controlling the
// corners of the board.
static int evaluate_corners(Game *game)
{
    int score = 0;

    // Indices of the corners of the board.
    int indices[2] = { 0, BOARD_SIZE-1 };

    // Iterate through the indices of the corners.
    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            if ((*game).board[indices[i]][indices[j]].status == full)
            {
                if ((*game).board[indices[i]][indices[j]].color == black)
                    score -= CORNER_VALUE;
                else
                    score += CORNER_VALUE;
            }
        }
    }
    // Return the score.
    return score;
}
