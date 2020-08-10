#ifndef SHOP
#define SHOP

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define SHOPS_CNT 5
#define BUYER_CNT 3

#define PRODUCT_CNT_MIN 450
#define PRODUCT_CNT_MAX 550
#define PRODUCT_CNT_LOAD 500

#define NEED_MIN 9800
#define NEED_MAX 10200

typedef struct shop shop;

struct shop {
    pthread_mutex_t mutex;
    int product_cnt;
};

int get_rand_num(int, int);
void init();
void *buyer(void *);
void *loader(void *);
void run();

#endif