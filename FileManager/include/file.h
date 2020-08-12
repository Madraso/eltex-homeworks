#ifndef FILE
#define FILE

#include "../include/headers.h"

enum file_type { DIRECT = 1, REG = 2, EXEC = 3 };

void color_print(WINDOW *, char *, int);
struct stat get_file_stat(struct working_wnd *, int);
mode_t get_file_perms(char *);
void get_next_dir(struct working_wnd *, char *);
int define_file_type(struct working_wnd *, int);
int check_access(struct path);
void *copy_file(void *);
void print_all_files(struct working_wnd *);
void print_file(struct working_wnd *, int);

#endif