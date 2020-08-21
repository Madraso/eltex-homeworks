#ifndef CLIENT
#define CLIENT

#include <sys/types.h>
#include <sys/msg.h>
#include <unistd.h>
#include <locale.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <ncurses.h>

#define NICKNAME_LEN 16
#define MSG_LEN 256
#define CL_CNT 100
#define MSG_CNT 500

struct msgbuf {
    long mtype;
    unsigned char mtext[MSG_LEN];
};

void get_token(unsigned char *, char *, int);
void check_value(int, const char *);
void init_windows();
void exit_client();
void get_queues_id();
int hdl_serv_msg(unsigned char *);
void *hdl_chat_wnd(void *);
void *hdl_msg_wnd(void *);

#endif