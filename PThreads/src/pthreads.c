#include <stdio.h>
#include <pthread.h>

#define CNT_THREADS 1000

long long int num = 0;
pthread_mutex_t mutex;

void *increment_num(void *args) {
    long long int tmp;
    pthread_mutex_lock(&mutex);
    for (int i = 0; i < 1000000; i++) {
        tmp = num;
        tmp++;
        num = tmp;
    }
    pthread_mutex_unlock(&mutex);
}

int main() {
    pthread_t tid[CNT_THREADS];
    pthread_mutex_init(&mutex, NULL);
    for (int i = 0; i < CNT_THREADS; i++) {
        pthread_create(&tid[i], NULL, increment_num, NULL);
    }
    for (int i = 0; i < CNT_THREADS; i++) pthread_join(tid[i], NULL);
    printf("Num = %lld\n", num);
    return 0;
}