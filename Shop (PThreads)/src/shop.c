#include "../include/shop.h"

shop shops[SHOPS_CNT];

int get_rand_num(int min, int max) {
    return rand() % (max + 1 - min) + min;
}

void init() {
    for (int i = 0; i < SHOPS_CNT; i++) {
        pthread_mutex_init(&shops[i].mutex, NULL);
        shops[i].product_cnt = get_rand_num(PRODUCT_CNT_MIN, PRODUCT_CNT_MAX);
    }
}

void *buyer(void *args) {
    int need = get_rand_num(NEED_MIN, NEED_MAX);
    int id = *((int *) args);
    int shop_num;
    printf("(Buyer %d) - start need: %d\n", id, need);
    while (need > 0) {
        shop_num = rand() % SHOPS_CNT;
        pthread_mutex_lock(&shops[shop_num].mutex);
        printf("(Buyer %d) - entered the store %d. Products in shop: %d\n",
            id, shop_num + 1, shops[shop_num].product_cnt);
        need -= shops[shop_num].product_cnt;
        shops[shop_num].product_cnt -= shops[shop_num].product_cnt;
        if (need < 0) {
            shops[shop_num].product_cnt = -need;
            need = 0;
            printf("(Buyer %d) - satisfied all the needs\n", id);
        }
        else {
            printf("(Buyer %d) - bought products in %d shop. Total count products in shop: %d. Current need: %d\n",
                id, shop_num + 1, shops[shop_num].product_cnt, need);
        }
        pthread_mutex_unlock(&shops[shop_num].mutex);
        sleep(2);
    }
}

void *loader(void *args) {
    int shop_num;
    while (1) {
        shop_num = rand() % SHOPS_CNT;
        pthread_mutex_lock(&shops[shop_num].mutex);
        printf("(Loader) - shop %d have %d products\n", shop_num + 1, shops[shop_num].product_cnt);
        shops[shop_num].product_cnt += PRODUCT_CNT_LOAD;
        printf("(Loader) - shop %d loaded with %d products. Total count products in shop: %d\n",
            shop_num + 1, PRODUCT_CNT_LOAD, shops[shop_num].product_cnt);
        pthread_mutex_unlock(&shops[shop_num].mutex);
        sleep(1);
    }
}

void run() {
    init();
    pthread_t buyer_tid[BUYER_CNT], loader_tid;
    int buyer_id[BUYER_CNT];
    for (int i = 0; i < BUYER_CNT; i++) {
        buyer_id[i] = i + 1;
        pthread_create(&buyer_tid[i], NULL, buyer, (void *) &buyer_id[i]);
    }
    pthread_create(&loader_tid, NULL, loader, NULL);
    for (int i = 0; i < BUYER_CNT; i++) {
        pthread_join(buyer_tid[i], NULL);
    }
    pthread_cancel(loader_tid);
}