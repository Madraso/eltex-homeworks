#include "../include/defines.h"

void check_value(int val, const char *mes) {
    if (val < 0) {
        perror(mes);
        exit(-1);
    }
}

int conn_serv(struct sockaddr_in *st) {
    struct sockaddr_in server;

    memset(&server, 0, sizeof(server));

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(SERV_ADDR);
    server.sin_port = htons(SERV_PORT);

    int sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    check_value(sock_fd, "socket");

    sendto(sock_fd, "connect", 7, 0, (struct sockaddr *) &server, sizeof(server));
    recvfrom(sock_fd, st, sizeof(*st), 0, NULL, NULL);

    return sock_fd;
}

int main() {
    setlocale(LC_ALL, "");
    system("clear");

    struct sockaddr_in new_server;
    char buffer[BUF_LEN];

    int sock_fd = conn_serv(&new_server), st_size;

    printf("Подключение к основному серверу прошло успешно\n\n");
    printf("Основной сервер прислал endpoint нового сервера\n\n");

    st_size = sizeof(new_server);

    while (1) {
        printf("$ ");
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strlen(buffer) - 1] = 0;
        if (strlen(buffer) == 0) continue;
        printf("Клиент отправил сообщение: %s\n", buffer);
        sendto(sock_fd, buffer, sizeof(buffer), 0, (struct sockaddr *) &new_server, st_size);
        if (strcmp(buffer, "/exit") == 0) {
            close(sock_fd);
            exit(0);
        }
        recvfrom(sock_fd, buffer, sizeof(buffer), 0, (struct sockaddr *) &new_server, &st_size);
        printf("Сервер ответил: %s\n\n", buffer);
    }
}