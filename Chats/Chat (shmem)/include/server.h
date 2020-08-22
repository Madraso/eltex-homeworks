#ifndef SERVER
#define SERVER

#include <sys/mman.h>
#include <sys/types.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <locale.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

int getch();
void check_value(int, const char *);
void *hdl_serv_keys(void *);

#endif