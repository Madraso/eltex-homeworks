#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <locale.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

void check_value(int val, const char *mes) {
    if (val < 0) {
        perror(mes);
        exit(-1);
    }
}

int main() {
    system("clear");
    setlocale(LC_ALL, "");

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_port = htons(8080);

    int sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    check_value(sock_fd, "socket");
    printf("Создан сокет\n");

    int con_val = connect(sock_fd, (struct sockaddr *) &server, sizeof(server));
    check_value(con_val, "connect");
    printf("Произошло соединение с сервером\n");

    unsigned char buffer[256] = "Hello! ";
    
    send(sock_fd, buffer, sizeof(buffer), 0);
    printf("Клиент отправил сообщение: %s\n", buffer);
    recv(sock_fd, buffer, sizeof(buffer), 0);
    printf("Сервер прислал сообщение: %s\n", buffer);
    close(sock_fd);
}