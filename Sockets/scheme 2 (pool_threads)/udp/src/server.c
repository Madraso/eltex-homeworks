#include "../include/defines.h"

struct serv {
    struct sockaddr_in endpoint;
    char is_free;
} servers[THREAD_CNT];

struct th_inf {
    int thread_num;
    int sock_fd;
} th_args[THREAD_CNT];

pthread_mutex_t mutex;

void check_value(int val, const char *mes) {
    if (val < 0) {
        perror(mes);
        exit(-1);
    }
}

int creat_sock(struct sockaddr_in *st, int port) {
    memset(st, 0, sizeof(*st));

    st->sin_family = AF_INET;
    st->sin_addr.s_addr = inet_addr(SERV_ADDR);
    if (port != 0) st->sin_port = htons(port);

    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    check_value(fd, "socket");
    
    int bind_val = bind(fd, (struct sockaddr *) st, sizeof(*st));
    check_value(bind_val, "bind");

    int st_size = sizeof(*st);
    int getsock_val = getsockname(fd, (struct sockaddr *) st, &st_size);
    check_value(getsock_val, "getsockname");

    return fd;
}

void *hdl_input(void *args) {
    int *serv_fd = (int *) args;
    char buffer[BUF_LEN];

    while (1) {
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strlen(buffer) - 1] = 0;
        if (strlen(buffer) == 0) continue;
        if (strcmp(buffer, "/exit") == 0) {
            close(*serv_fd);
            for (int i = 0; i < THREAD_CNT; i++) {
                close(th_args[i].sock_fd);
            }
            pthread_mutex_destroy(&mutex);
            exit(0);
        }
    }
}

void *hdl_client(void *args) {
    struct th_inf *arr = (struct th_inf *) args;
    struct sockaddr_in client;
    char buffer[BUF_LEN];

    int st_size = sizeof(client);
    
    while (1) {
        recvfrom(arr->sock_fd, buffer, sizeof(buffer), 0, (struct sockaddr *) &client, &st_size);
        if (strcmp(buffer, "/exit") == 0) {
            printf("Клиент отключился от сервера\n\n");
            if (arr->thread_num == THREAD_CNT) {
                pthread_exit(NULL);
            }
            else {
                pthread_mutex_lock(&mutex);
                servers[arr->thread_num].is_free = 1;
                pthread_mutex_unlock(&mutex);
                continue;
            }
        }
        printf("Клиент прислал сообщение: %s\n", buffer);
        strcat(buffer, " (server)");
        printf("Сервер ответил: %s\n\n", buffer);
        sendto(arr->sock_fd, buffer, sizeof(buffer), 0, (struct sockaddr *) &client, st_size);
    }
}

int main() {
    setlocale(LC_ALL, "");
    system("clear");

    pthread_t tid;
    struct sockaddr_in server, client, new_server;
    char buffer[BUF_LEN];

    int serv_fd = creat_sock(&server, SERV_PORT), st_size = sizeof(client);
    
    printf("Основной сервер создан\n\n");

    pthread_create(&tid, NULL, hdl_input, (void *) &serv_fd);

    pthread_mutex_init(&mutex, NULL);

    for (int i = 0; i < THREAD_CNT; i++) {
        th_args[i].sock_fd = creat_sock(&servers[i].endpoint, 0);
        th_args[i].thread_num = i;
        servers[i].is_free = 1;
        pthread_create(&tid, NULL, hdl_client, (void *) &th_args[i]);
    }

    while (1) {
        recvfrom(serv_fd, buffer, sizeof(buffer), 0, (struct sockaddr *) &client, &st_size);

        printf("Подключился новый клиент\n\n");
        
        int i = 0;
        for (; i < THREAD_CNT; i++) {
            if (servers[i].is_free) {
                servers[i].is_free = 0;
                sendto(serv_fd, &servers[i].endpoint,
                    sizeof(servers[i].endpoint), 0, (struct sockaddr *) &client, st_size);
                break;
            }
        }
        if (i == THREAD_CNT) {
            struct th_inf args;
            args.sock_fd = creat_sock(&new_server, 0);
            args.thread_num = THREAD_CNT;
            pthread_create(&tid, NULL, hdl_client, (void *) &args);
            sendto(serv_fd, &new_server,
                    sizeof(new_server), 0, (struct sockaddr *) &client, st_size);
        }
    }
}
