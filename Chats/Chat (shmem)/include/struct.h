#ifndef STRUCT
#define STRUCT

#include <semaphore.h>

#define NICKNAME_LEN 16
#define MSG_LEN 256
#define CL_CNT 100
#define MSG_CNT 500

#define SHM_NAME "/shmem"

struct chat {
    unsigned char user[CL_CNT][NICKNAME_LEN];
    unsigned char msg[MSG_CNT][MSG_LEN];
    sem_t sem_mutex, usr_cnt, msg_cnt;
    pthread_mutex_t pth_mutex;
    pthread_cond_t cond_chat, cond_users;
};

#endif