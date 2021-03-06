#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <locale.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#define MULTICAST_ADDR "224.0.0.1"
#define SERV_PORT 7000

void check_value(int val, const char *mes) {
    if (val < 0) {
        perror(mes);
        exit(-1);
    }
}

int main() {
    system("clear");
    setlocale(LC_ALL, "");

    struct sockaddr_in client;

    client.sin_family = AF_INET;
    client.sin_addr.s_addr = inet_addr(MULTICAST_ADDR);
    client.sin_port = htons(SERV_PORT);

    int sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    check_value(sock_fd, "socket");
    printf("UDP сокет создан\n\n");

    unsigned char buffer[256] = "Hello!";

    printf("Сервер готов передавать multicast данные\n\n");
    
    sendto(sock_fd, buffer, sizeof(buffer), 0, (struct sockaddr *) &client, sizeof(client));
    printf("Сервер отправил сообщение: %s\n", buffer);

    close(sock_fd);

    return 0;
}