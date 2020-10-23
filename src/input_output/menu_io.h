#ifndef _MENU_IO_
#define _MENU_IO_

void print_welcome_message(void);
void print_select_game_mode(void);
void print_select_game_mode_result(char selection);
void print_select_discs_color(Game *game);
void print_select_discs_color_result(Game *game);
char generate_random_bool(void);
int validate_integer(int min, int max);
void consume_buffer(void);

#endif
