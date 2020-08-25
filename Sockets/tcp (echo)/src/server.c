#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <locale.h>
#include <unistd.h>
#include <getopt.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void check_value(int val, const char *mes) {
    if (val < 0) {
        perror(mes);
        exit(-1);
    }
}

void hdl_args(struct sockaddr_in *endpoint, int argc, char* argv[]) {
    int symbol, entry_cnt = 0;

    while ((symbol = getopt(argc, argv, "a:p:")) != -1) {
        int index = 0;

        entry_cnt++;

        if (optarg != NULL) {
            while (optarg[index++] == ' ') optarg++;
        }

        switch (symbol) {
            case 'a': {
                struct hostent *host = gethostbyname(optarg);
                if (host == NULL) {
                    herror("gethostbyname");
                    exit(-1);
                }
                struct in_addr *addr = (struct in_addr *) host->h_addr_list[0];
                endpoint->sin_addr.s_addr = addr->s_addr;
            } break;
            case 'p': {
                endpoint->sin_port = htons(atoi(optarg));
            } break;
            case '?': {
                exit(-1);
            } break;
        }
    }

    if (entry_cnt < 2) {
        printf("Не указан адрес или порт\n");
        exit(-1);
    }
}

int main(int argc, char* argv[]) {
    system("clear");
    setlocale(LC_ALL, "");

    struct sockaddr_in server;
    memset(&server, 0, sizeof(struct sockaddr_in));
    
    server.sin_family = AF_INET;

    hdl_args(&server, argc, argv);

    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    check_value(sock_fd, "socket");
    printf("Создан сокет\n");

    int bind_val = bind(sock_fd, (struct sockaddr *) &server, sizeof(server));
    check_value(bind_val, "bind");
    printf("Сокет привязан к сетевому интерфейсу\n");

    int listen_val = listen(sock_fd, 5);
    check_value(listen_val, "listen");
    printf("Сокет переведен в пассивный режим\n");

    int new_fd = accept(sock_fd, NULL, NULL);
    check_value(new_fd, "accept");
    printf("Клиент подключился к серверу\n");

    unsigned char buffer[256];
    
    recv(new_fd, buffer, sizeof(buffer), 0);
    printf("Клиент прислал сообщение: %s\n", buffer);
    strcat(buffer, "(From server)");
    send(new_fd, buffer, sizeof(buffer), 0);
    printf("Сервер отправил сообщение: %s\n", buffer);
    close(new_fd);
    close(sock_fd);
}