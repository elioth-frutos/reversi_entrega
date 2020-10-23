#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../game.h"

void consume_buffer(void);


void print_welcome_message(void)
{
    printf("\nWelcome to Reversi.\n");
}


void print_select_game_mode(void)
{
    printf("\n\nSelect the game mode:\n");
    printf("1) Single player.\n");
    printf("2) Two players.\n");
    printf("3) The program against itself.\n");
    printf("4) The program against another program.");
    printf("\n\nYour selection: ");
}


void print_select_game_mode_result(char selection)
{
    switch (selection)
    {
        case 1:
            printf("\nYou will play against the CPU.\n");
            break;
        case 2:
            printf("\nYou will play against another person.\n");
            break;
        case 3:
            printf("\nThe CPU will play against itself.\n");
            break;
        case 4:
            printf("\nThe CPU will play against another program.\n");
            break;
    }
}


void print_select_first_player(Game *game)
{
    printf("\n\nSelect who will play first: \n");
    switch ((*game).mode)
    {
        case single_player:
            printf("1) You.\n");
            printf("2) The CPU.\n");
           break;
        case two_players:
            printf("1) You.\n");
            printf("2) Your opponent.\n");
           break;
        case cpu_vs_itself:
            printf("1) CPU 1.\n");
            printf("2) CPU 2.\n");
           break;
        case cpu_vs_another_cpu:
            printf("1) The CPU.\n");
            printf("2) The opponent's CPU.\n");
            break;
    }
    printf("3) Select randomly.");
    printf("\n\nYour selection: ");
}


void print_select_first_player_result(Game *game, char selection)
{
    if (selection == 1)
    {
        switch ((*game).mode)
        {
            case single_player:
            case two_players:
                printf("\nYou will play first.\n");
                break;
            case cpu_vs_itself:
                printf("\nThe CPU 1 will play first.\n");
                break;
            case cpu_vs_another_cpu:
                printf("\nThe CPU will play first.\n");
                break;
        }
    }
    else
    {
        switch ((*game).mode)
        {
            case single_player:
                printf("\nThe CPU will play first.\n");
                break;
            case two_players:
                printf("\nYour opponent will play first.\n");
                break;
            case cpu_vs_itself:
                printf("\nThe CPU 2 will play first.\n");
                break;
            case cpu_vs_another_cpu:
                printf("\nYour opponent's CPU will play first.\n");
                break;
        }
    }
}


void print_select_discs_color(Game *game)
{
    switch ((*game).mode)
    {
        case single_player:
        case two_players:
            printf("\n\nSelect the color of your discs:\n");
            break;
        case cpu_vs_itself:
            printf("\n\nSelect the color of the CPU 1's discs:\n");
            break;
        case cpu_vs_another_cpu:
            printf("\n\nSelect the color of the CPU's discs:\n");
            break;
    }
    printf("1) Black discs.\n");
    printf("2) White discs.\n");
    printf("3) Select randomly.");
    printf("\n\nYour selection: ");
}


void print_select_discs_color_result(Game *game)
{
    char *color_1;
    char *color_2;
    if ((*game).players_color.player_1 == white)
    {
        color_1 = "white";
        color_2 = "black";
    }
    else
    {
        color_1 = "black";
        color_2 = "white";
    }

    switch ((*game).mode)
    {
        case single_player:
            printf("\nYou will play with the %s discs.\n", color_1);
            printf("The CPU will play with the %s discs.\n", color_2);
            break;
        case two_players:
            printf("\nYou will play with the %s discs.\n", color_1);
            printf("Your opponent will play with the %s discs.\n", color_2);
            break;
        case cpu_vs_itself:
            printf("\nThe CPU 1 will play with the %s discs.\n", color_1);
            printf("The CPU 2 will play with the %s discs.\n", color_2);
            break;
        case cpu_vs_another_cpu:
            printf("\nThe CPU will play with the %s discs.\n", color_1);
            printf("Your opponent's CPU will play with the %s discs.\n", color_2);
            break;
    }
}


// Generates either a 0 or a 1 randomly.
char generate_random_bool(void)
{
    srand(time(NULL));
    return rand() % 2;
}


int validate_integer(int min, int max)
{
    int input;
    // Recursive case. No input was parsed.
    // Clean the buffer and ask again.
    if (!scanf("%d", &input))
    {
        consume_buffer();
        return validate_integer(min, max);
    }

    // Base case. Some input was parsed and it's
    // inside the valid range.
    if (input >= min && input <= max)
    {
        consume_buffer();
        return input;
    }

    // Recursive case. The input is outside the
    // valid range. Clean the buffer and ask again.
    consume_buffer();
    return validate_integer(min, max);
}


void consume_buffer(void)
{
    int input = getchar();
    // Base case. End of stdin buffer.
    if (input == '\n' || input == EOF)
        return;
    // Recursive case. Consume next character.
    return consume_buffer();
}
