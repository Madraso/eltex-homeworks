#ifndef MANAGER
#define MANAGER

#include "../include/headers.h"

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

struct path {
    char src[FILENAME_MAX + 256];
    char dst[FILENAME_MAX + 256];
};

WINDOW *msg_wnd;

int filter_names(const struct dirent *);
void init_screen();
void init_windows(struct working_wnd *[]);
void *show_message(void *);
void draw_ext_window(struct working_wnd *);
void draw_int_window(struct working_wnd *);
void draw_pointer(struct working_wnd *);
void run();

#endif