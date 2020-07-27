#ifndef TEXT_EDITOR
#define TEXT_EDITOR

#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>
#include <locale.h>
#include <curses.h>

#define MAX_STRING_COUNT 1000

enum direction { UP = -1, DOWN = 1, ALL = 0 };
enum file_mode { OPEN, SAVE };

void init_screen();
void op_sv_file();
void read_file();
void scroll_text(int);
void display_content(int, int, int);
void draw_toolbar(int, int);
void set_canon_mode();
void set_uncanon_mode();
void new_string(int);
void run();

#endif