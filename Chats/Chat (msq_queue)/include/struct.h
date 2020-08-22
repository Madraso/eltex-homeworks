#ifndef STRUCT
#define STRUCT

#define NICKNAME_LEN 16
#define MSG_LEN 256
#define CL_CNT 100
#define MSG_CNT 500

struct msgbuf {
    long mtype;
    unsigned char mtext[MSG_LEN];
};

#endif