#ifndef SERVER
#define SERVER

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <termios.h>
#include <unistd.h>
#include <locale.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>

int getch();
void get_token(unsigned char *);
void arr_shift(char *, int, int);
void check_value(int, const char *);
void snd_nicknames();
void snd_msg_hist();
void *hdl_serv_keys(void *);
void *hdl_serv_queue(void *);

#endif