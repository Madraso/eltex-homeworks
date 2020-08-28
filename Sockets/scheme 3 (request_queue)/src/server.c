#include "../include/defines.h"

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
            mq_unlink("/msg_queue");
            exit(0);
        }
    }
}

void *hdl_client(void *args) {
    mqd_t *q_id = (mqd_t *) args;
    char buffer[BUF_LEN];

    struct mq_attr attr;
    int getattr_val = mq_getattr(*q_id, &attr);
    check_value(getattr_val, "mq_getattr");

    char str_fd[attr.mq_msgsize + 1];

    mq_receive(*q_id, str_fd, sizeof(str_fd), NULL);
    printf("Клиент начал обслуживаться потоком\n\n");

    int cl_fd = atoi(str_fd);
    
    while (1) {
        recv(cl_fd, buffer, sizeof(buffer), 0);
        if (strcmp(buffer, "/exit") == 0) {
            printf("Клиент отключился от сервера\n\n");
            pthread_exit(NULL);
        }
        printf("Клиент прислал сообщение: %s\n", buffer);
        strcat(buffer, " (server)");
        printf("Сервер ответил: %s\n\n", buffer);
        send(cl_fd, buffer, sizeof(buffer), 0);
    }
}

int main() {
    setlocale(LC_ALL, "");
    system("clear");

    pthread_t tid;
    struct sockaddr_in server;

    int serv_fd = creat_sock(&server, SOCK_STREAM, SERV_PORT);

    printf("Основной сервер создан\n\n");

    mqd_t q_id = mq_open("/msg_queue", O_CREAT | O_RDWR, 0666, NULL);
    check_value(q_id, "mq_open");

    printf("Очередь заявок создана\n\n");

    int args[2];
    args[0] = serv_fd;
    args[1] = q_id;
    pthread_create(&tid, NULL, hdl_input, (void *) &args);

    for (int i = 0; i < THREAD_CNT; i++) {
        pthread_create(&tid, NULL, hdl_client, (void *) &q_id);
    }

    char str_fd[10];
    int cl_cnt = 0;

    while (1) {
        int cl_fd = accept(serv_fd, NULL, NULL);
        check_value(cl_fd, "accept");

        printf("Подключился новый клиент\n\n");

        cl_cnt++;

        sprintf(str_fd, "%d", cl_fd);
        mq_send(q_id, str_fd, sizeof(str_fd), 1);
        
        if (cl_cnt > THREAD_CNT) {
            for (int i = 0; i < THREAD_CNT; i++) {
                pthread_create(&tid, NULL, hdl_client, (void *) &q_id);
            }
            cl_cnt = 0;
        }
    }
}