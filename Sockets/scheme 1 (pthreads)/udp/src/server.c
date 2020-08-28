#include "../include/defines.h"

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
        printf("$ ");
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
    int *cl_fd = (int *) args;
    struct sockaddr_in client;
    char buffer[BUF_LEN];

    int st_size = sizeof(client);
    
    while (1) {
        recvfrom(*cl_fd, buffer, sizeof(buffer), 0, (struct sockaddr *) &client, &st_size);
        if (strcmp(buffer, "/exit") == 0) {
            printf("Клиент отключился от сервера\n\n");
            pthread_exit(NULL);
        }
        printf("Клиент прислал сообщение: %s\n", buffer);
        strcat(buffer, " (server)");
        printf("Сервер ответил: %s\n\n", buffer);
        sendto(*cl_fd, buffer, sizeof(buffer), 0, (struct sockaddr *) &client, st_size);
    }
}

int main() {
    setlocale(LC_ALL, "");
    system("clear");

    struct sockaddr_in server, client, new_server;

    int serv_fd = creat_sock(&server, SERV_PORT);
    
    printf("Основной сервер создан\n\n");

    pthread_t tid;
    pthread_create(&tid, NULL, hdl_input, (void *) &serv_fd);

    char buffer[BUF_LEN];
    int st_size = sizeof(client);

    while (1) {
        recvfrom(serv_fd, buffer, sizeof(buffer), 0, (struct sockaddr *) &client, &st_size);
        int cl_fd = creat_sock(&new_server, 0);
        printf("Подключился новый клиент\n\n");
        sendto(serv_fd, &new_server, sizeof(new_server), 0, (struct sockaddr *) &client, st_size);

        pthread_create(&tid, NULL, hdl_client, (void *) &cl_fd);
    }
}