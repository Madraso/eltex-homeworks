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

    struct sockaddr_in server;

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(SERV_PORT);

    int sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    check_value(sock_fd, "socket");
    printf("UDP сокет создан\n\n");

    struct ip_mreqn multicast;
    multicast.imr_multiaddr.s_addr = inet_addr(MULTICAST_ADDR);
    multicast.imr_address.s_addr = INADDR_ANY;
    multicast.imr_ifindex = 0;

    int setsockopt_val = setsockopt(sock_fd, IPPROTO_IP,
        IP_ADD_MEMBERSHIP, (void *) &multicast, sizeof(multicast));
    check_value(setsockopt_val, "setsockopt");

    int bind_val = bind(sock_fd, (struct sockaddr *) &server, sizeof(server));
    check_value(bind_val, "bind");

    printf("Клиент готов принимать сообщения от сервера\n\n");

    unsigned char buffer[256];
    int srv_st_size = sizeof(server);
    
    recvfrom(sock_fd, buffer, sizeof(buffer), 0, (struct sockaddr *) &server, &srv_st_size);
    printf("Клиент принял сообщение: %s\n", buffer);

    close(sock_fd);
    
    return 0;
}