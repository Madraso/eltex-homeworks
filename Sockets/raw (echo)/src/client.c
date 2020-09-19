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

    struct sockaddr_in server;
    memset(&server, 0, sizeof(struct sockaddr_in));

    server.sin_family = AF_INET;

    hdl_args(&server, argc, argv);

    int sock_fd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
    check_value(sock_fd, "socket");
    printf("RAW сокет успешно создан\n\n");

    int iphdr_size = sizeof(struct iphdr), udphdr_size = sizeof(struct udphdr);
    int serv_st_size = sizeof(server);
    
    char buf_snd[udphdr_size + BUF_LEN],
        buf_rcv[udphdr_size + iphdr_size + BUF_LEN];
    
    char *buf_in = buf_snd + udphdr_size;
    strcpy(buf_in, "Hello!");

    struct udphdr *udp_hdr = (struct udphdr *) buf_snd;
    udp_hdr->source = htons(port + 1000);
    udp_hdr->dest = htons(port);
    udp_hdr->len = htons(udphdr_size + strlen(buf_in));
    udp_hdr->check = htons(0);
    
    sendto(sock_fd, buf_snd, udp_hdr->len, 0, (struct sockaddr *) &server, serv_st_size);
    printf("Клиент отправил сообщение: %s\n", buf_in);

    int cnt_msg = 0;
    while (1) {
        recvfrom(sock_fd, buf_rcv, sizeof(buf_rcv), 0, (struct sockaddr *) &server, &serv_st_size);
        printf("Сервер прислал сообщение: %s\n\n", buf_rcv + iphdr_size + udphdr_size);
        if (++cnt_msg == 2) break;
    }

    return 0;
}