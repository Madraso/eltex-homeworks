#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <linux/ip.h>
#include <linux/udp.h>
#include <locale.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#define BUF_LEN 256

struct message {
    struct iphdr ip_hdr;
    struct udphdr udp_hdr;
    unsigned char mes[BUF_LEN];
};

void check_value(int val, const char *msg) {
    if (val < 0) {
        perror(msg);
        exit(-1);
    }
}

int main() {
    system("clear");
    setlocale(LC_ALL, "");

    int sock_fd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
    check_value(sock_fd, "socket");
    printf("RAW сокет успешно создан\n\n");
    
    struct message buffer;
    struct in_addr addr;

    printf("Ожидание пакетов...\n\n");
    while (1) {
        recv(sock_fd, &buffer, sizeof(buffer), 0);
        printf("Принят UDP пакет\n");

        addr.s_addr = buffer.ip_hdr.saddr;
        printf("Адрес источника - %s:%d\n", inet_ntoa(addr), ntohs(buffer.udp_hdr.source));

        addr.s_addr = buffer.ip_hdr.daddr;
        printf("Адрес назначения - %s:%d\n\n", inet_ntoa(addr), ntohs(buffer.udp_hdr.dest));
    }
}