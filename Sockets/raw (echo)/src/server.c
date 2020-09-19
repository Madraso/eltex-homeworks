#include "../include/defines.h"

char *ip;
int port;

void check_value(int val, const char *msg) {
    if (val < 0) {
        perror(msg);
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
                ip = optarg;
                if (host == NULL) {
                    herror("gethostbyname");
                    exit(-1);
                }
                struct in_addr *addr = (struct in_addr *) host->h_addr_list[0];
                endpoint->sin_addr.s_addr = addr->s_addr;
            } break;
            case 'p': {
                endpoint->sin_port = htons(atoi(optarg));
                port = atoi(optarg);
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

    struct sockaddr_in server, client;
    memset(&server, 0, sizeof(struct sockaddr_in));

    server.sin_family = AF_INET;

    hdl_args(&server, argc, argv);

    int sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    check_value(sock_fd, "socket");
    printf("UDP сокет успешно создан\n");

    int bind_val = bind(sock_fd, (struct sockaddr *) &server, sizeof(server));
    check_value(bind_val, "bind");
    printf("Сокет привязан к адресу %s:%d\n\n", ip, port);

    unsigned char buffer[BUF_LEN];
    int cl_size = sizeof(client);

    recvfrom(sock_fd, buffer, sizeof(buffer), 0, (struct sockaddr *) &client, &cl_size);
    printf("Клиент прислал сообщение: %s\n", buffer);

    strcat(buffer, " (From server)");
    sendto(sock_fd, buffer, sizeof(buffer), 0, (struct sockaddr *) &client, cl_size);
    printf("Сервер отправил сообщение: %s\n\n", buffer);

    return 0;
}