#ifndef FILE_MANAGER
#define FILE_MANAGER

#include <ncurses.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <time.h>
#include <locale.h>
#include <stdlib.h>
#include <unistd.h>
#include <malloc.h>
#include <dirent.h>

enum direction { UP = -1, DOWN = 1, ALL = 0 };
typedef struct working_wnd working_wnd;

struct working_wnd {
    struct dirent **namelist;
    WINDOW *wnd;
    WINDOW *subwnd;
    int file_num;
    int entry_cnt;
    unsigned char pwd[FILENAME_MAX];
    unsigned char change_dir;
    unsigned char is_active;
};

int filter_names(const struct dirent *);
void color_print(WINDOW *, char *, int);
void init_screen();
void init_windows(struct working_wnd *[]);
struct stat get_file_stat(struct working_wnd *, int);
void get_next_dir(struct working_wnd *, char *);
void draw_ext_window(struct working_wnd *);
void draw_int_window(struct working_wnd *);
void draw_pointer(struct working_wnd *);
void print_all_files(struct working_wnd *);
int define_file_type(struct working_wnd *, int);
void print_file(struct working_wnd *, int);
void run();

#endif