#include "../include/defines.h"

struct serv {
    struct sockaddr_in endpoint;
    sem_t sem;
    int fd;
} pool_tcp_serv[THREAD_CNT / 2], pool_udp_serv[THREAD_CNT / 2];

void check_value(int val, const char *mes) {
    if (val < 0) {
        perror(mes);
        exit(-1);
    }
}

int creat_sock(struct sockaddr_in *st, int prot, int port) {
    memset(st, 0, sizeof(*st));

    st->sin_family = AF_INET;
    st->sin_addr.s_addr = inet_addr(SERV_ADDR);
    if (port != 0) st->sin_port = htons(port);

    int fd = socket(AF_INET, prot, 0);
    check_value(fd, "socket");
    
    int bind_val = bind(fd, (struct sockaddr *) st, sizeof(*st));
    check_value(bind_val, "bind");

    if (prot == SOCK_STREAM) {
        int listen_val = listen(fd, 5);
        check_value(listen_val, "listen");
    }

    int st_size = sizeof(*st);
    int getsock_val = getsockname(fd, (struct sockaddr *) st, &st_size);
    check_value(getsock_val, "getsockname");

    return fd;
}

void *hdl_input(void *args) {
    int *th_args = (int *) args;
    char buffer[BUF_LEN];

    while (1) {
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strlen(buffer) - 1] = 0;
        if (strlen(buffer) == 0) continue;
        if (strcmp(buffer, "/exit") == 0) {
            close(th_args[0]);
            close(th_args[1]);
            for (int i = 0; i < (THREAD_CNT / 2); i++) {
                sem_destroy(&pool_tcp_serv[i].sem);
                close(pool_tcp_serv[i].fd);
                close(pool_udp_serv[i].fd);
            }
            exit(0);
        }
    }
}

void *hdl_tcp_client(void *args) {
    struct serv *arr = (struct serv *) args;
    char buffer[BUF_LEN];
    
    sem_wait(&arr->sem);

    while (1) {
        recv(arr->fd, buffer, sizeof(buffer), 0);
        if (strcmp(buffer, "/exit") == 0) {
            printf("TCP клиент отключился от сервера\n\n");
            pthread_exit(NULL);
        }
        printf("TCP клиент прислал сообщение: %s\n", buffer);
        strcat(buffer, " (server)");
        printf("TCP сервер ответил: %s\n\n", buffer);
        send(arr->fd, buffer, sizeof(buffer), 0);
    }
}

void *hdl_udp_client(void *args) {
    struct serv *arr = (struct serv *) args;
    struct sockaddr_in client;
    char buffer[BUF_LEN];

    int st_size = sizeof(client);
    
    while (1) {
        recvfrom(arr->fd, buffer, sizeof(buffer), 0, (struct sockaddr *) &client, &st_size);
        if (strcmp(buffer, "/exit") == 0) {
            printf("UDP клиент отключился от сервера\n\n");
            pthread_exit(NULL);
        }
        printf("UDP клиент прислал сообщение: %s\n", buffer);
        strcat(buffer, " (server)");
        printf("UDP сервер ответил: %s\n\n", buffer);
        sendto(arr->fd, buffer, sizeof(buffer), 0, (struct sockaddr *) &client, st_size);
    }
}

int main() {
    setlocale(LC_ALL, "");
    system("clear");

    pthread_t tid;
    struct sockaddr_in tcp_server, udp_server, client;
    struct pollfd pollst[2];

    int tcp_fd = creat_sock(&tcp_server, SOCK_STREAM, SERV_PORT);
    int udp_fd = creat_sock(&udp_server, SOCK_DGRAM, SERV_PORT);

    printf("Основные TCP и UDP сервера созданы\n\n");

    pollst[0].fd = tcp_fd;
    pollst[0].events = POLLIN;
    pollst[1].fd = udp_fd;
    pollst[1].events = POLLIN;
    
    int args[2];
    args[0] = tcp_fd;
    args[1] = udp_fd;
    pthread_create(&tid, NULL, hdl_input, (void *) &args);

    for (int i = 0; i < (THREAD_CNT / 2); i++) {
        sem_init(&pool_tcp_serv[i].sem, 0, 0);
        pthread_create(&tid, NULL, hdl_tcp_client, (void *) &pool_tcp_serv[i]);

        pool_udp_serv[i].fd = creat_sock(&pool_udp_serv[i].endpoint, SOCK_DGRAM, 0);
        pthread_create(&tid, NULL, hdl_udp_client, (void *) &pool_udp_serv[i]);
    }

    char buffer[BUF_LEN];
    int st_size = sizeof(client);

    int tcp_cl_cnt = 0, udp_cl_cnt = 0;

    while (1) {
        int poll_val = poll(pollst, 2, 0);
        check_value(poll_val, "poll");
        
        if (pollst[0].revents & POLLIN) {
            int cl_fd = accept(tcp_fd, NULL, NULL);
            check_value(cl_fd, "accept");

            printf("Подключился новый TCP клиент\n\n");

            tcp_cl_cnt++;

            if (tcp_cl_cnt > (THREAD_CNT / 2)) {
                struct serv args;
                sem_init(&args.sem, 0, 0);
                args.fd = cl_fd;
                pthread_create(&tid, NULL, hdl_tcp_client, (void *) &args);
                sem_post(&args.sem);
            }
            else {
                pool_tcp_serv[tcp_cl_cnt - 1].fd = cl_fd;
                sem_post(&pool_tcp_serv[tcp_cl_cnt - 1].sem);
            }
        }
        else if (pollst[1].revents & POLLIN) {
            recvfrom(udp_fd, buffer, sizeof(buffer), 0, (struct sockaddr *) &client, &st_size);

            printf("Подключился новый UDP клиент\n\n");

            udp_cl_cnt++;

            if (udp_cl_cnt > (THREAD_CNT / 2)) {
                struct serv args;
                args.fd = creat_sock(&args.endpoint, SOCK_DGRAM, 0);
                pthread_create(&tid, NULL, hdl_udp_client, (void *) &args);
                sendto(udp_fd, &args.endpoint,
                    sizeof(args.endpoint), 0, (struct sockaddr *) &client, st_size);
            }
            else {
                sendto(udp_fd, &pool_udp_serv[udp_cl_cnt - 1].endpoint,
                        sizeof(pool_udp_serv[udp_cl_cnt - 1].endpoint), 0, (struct sockaddr *) &client, st_size);
            }
        }
    }
}