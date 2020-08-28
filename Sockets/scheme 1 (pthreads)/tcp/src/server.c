#include "../include/defines.h"

struct serv {
    sem_t sem;
    int fd;
};

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

    int fd = socket(AF_INET, SOCK_STREAM, 0);
    check_value(fd, "socket");
    
    int bind_val = bind(fd, (struct sockaddr *) st, sizeof(*st));
    check_value(bind_val, "bind");

    int listen_val = listen(fd, 5);
    check_value(listen_val, "listen");

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
            exit(0);
        }
    }
}

void *hdl_client(void *args) {
    struct serv *arr = (struct serv *) args;
    char buffer[BUF_LEN];

    sem_wait(&arr->sem);
    
    while (1) {
        recv(arr->fd, buffer, sizeof(buffer), 0);
        if (strcmp(buffer, "/exit") == 0) {
            printf("Клиент отключился от сервера\n\n");
            pthread_exit(NULL);
        }
        printf("Клиент прислал сообщение: %s\n", buffer);
        strcat(buffer, " (server)");
        printf("Сервер ответил: %s\n\n", buffer);
        send(arr->fd, buffer, sizeof(buffer), 0);
    }
}

int main() {
    setlocale(LC_ALL, "");
    system("clear");

    pthread_t tid;
    struct sockaddr_in server;

    int serv_fd = creat_sock(&server, SERV_PORT);
    
    printf("Основной сервер создан\n\n");

    pthread_create(&tid, NULL, hdl_input, (void *) &serv_fd);

    while (1) {
        int cl_fd = accept(serv_fd, NULL, NULL);
        check_value(cl_fd, "accept");

        printf("Подключился новый клиент\n\n");

        struct serv args;
        sem_init(&args.sem, 0, 0);
        args.fd = cl_fd;
        pthread_create(&tid, NULL, hdl_client, (void *) &args);
        sleep(1);
        sem_post(&args.sem);
    }
}