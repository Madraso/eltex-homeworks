#ifndef DEFINES
#define DEFINES

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/poll.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>
#include <locale.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define SERV_ADDR "127.0.0.1"
#define SERV_PORT 8080
#define BUF_LEN 256
#define THREAD_CNT 4

#endif