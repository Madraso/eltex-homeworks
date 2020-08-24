#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <locale.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
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

    struct sockaddr_in server, client;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_port = htons(8080);

    int sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    check_value(sock_fd, "socket");
    printf("Создан сокет\n");

    int bind_val = bind(sock_fd, (struct sockaddr *) &server, sizeof(server));
    check_value(bind_val, "bind");
    printf("Сокет привязан к сетевому интерфейсу\n");

    unsigned char buffer[256];
    int cl_size = sizeof(client);
    
    recvfrom(sock_fd, buffer, sizeof(buffer), 0, (struct sockaddr *) &client, &cl_size);
    printf("Клиент прислал сообщение: %s\n", buffer);
    strcat(buffer, "(From server)");
    sendto(sock_fd, buffer, sizeof(buffer), 0, (struct sockaddr *) &client, cl_size);
    printf("Сервер отправил сообщение: %s\n", buffer);
    close(sock_fd);
}