#ifndef FILE_MANAGER
#define FILE_MANAGER

#include <ncurses.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <locale.h>
#include <stdlib.h>
#include <unistd.h>
#include <malloc.h>
#include <dirent.h>

struct filelist {
    struct dirent **namelist;
    WINDOW *wnd;
    int scrl_border[2];
    int file_ptr;
    int entry_cnt;
    unsigned char pwd[FILENAME_MAX];
    unsigned char change_dir;
    unsigned char is_active;
};

int filter_names(const struct dirent *);
int cmp_names(const void *, const void *);
void init_screen();
void init_windows(struct filelist *[]);
void read_dir(struct filelist *);
void define_file_types(struct filelist *, int);
void string_shift(char *);
struct stat get_file_stat(struct filelist *, int);
void get_next_dir(struct filelist *, char *);
void print_top_line(struct filelist *);
void print_file_inf(struct filelist *, int);
void run();

#endif