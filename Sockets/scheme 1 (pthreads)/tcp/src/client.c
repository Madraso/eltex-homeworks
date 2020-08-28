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

    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    check_value(sock_fd, "socket");

    int con_val = connect(sock_fd, (struct sockaddr *) &server, sizeof(server));
    check_value(con_val, "connect");

    return sock_fd;
}

int main() {
    setlocale(LC_ALL, "");
    system("clear");

    struct sockaddr_in new_server;
    char buffer[BUF_LEN];

    int sock_fd = conn_serv(&new_server);

    printf("Подключение к основному серверу прошло успешно\n\n");

    while (1) {
        printf("$ ");
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strlen(buffer) - 1] = 0;
        if (strlen(buffer) == 0) continue;
        printf("Клиент отправил сообщение: %s\n", buffer);
        send(sock_fd, buffer, sizeof(buffer), 0);
        if (strcmp(buffer, "/exit") == 0) {
            close(sock_fd);
            exit(0);
        }
        recv(sock_fd, buffer, sizeof(buffer), 0);
        printf("Сервер ответил: %s\n\n", buffer);
    }
}