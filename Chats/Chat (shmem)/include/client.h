#ifndef CLIENT
#define CLIENT

#include <sys/mman.h>
#include <fcntl.h>
#include <locale.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <ncurses.h>

void get_token(unsigned char *, char *, int);
void check_value(int, const char *);
void init_windows();
void exit_client();
void shmem_attach();
void *hdl_chat_wnd(void *);
void *hdl_users_wnd(void *);
void *hdl_msg_wnd(void *);

#endif