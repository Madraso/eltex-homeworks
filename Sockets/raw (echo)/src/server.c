#include "../include/defines.h"

void check_value(int val, const char *msg) {
    if (val < 0) {
        perror(msg);
        exit(-1);
    }
}

int main(int argc, char* argv[]) {
    system("clear");
    setlocale(LC_ALL, "");

    struct sockaddr_in server, client;
    memset(&server, 0, sizeof(struct sockaddr_in));

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(DEST_PORT);

    int sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    check_value(sock_fd, "socket");
    printf("UDP сокет успешно создан\n");

    int bind_val = bind(sock_fd, (struct sockaddr *) &server, sizeof(server));
    check_value(bind_val, "bind");

    unsigned char buffer[MTU];
    int cl_size = sizeof(client);

    memset(buffer, 0, sizeof(buffer));
    recvfrom(sock_fd, buffer, sizeof(buffer), 0, (struct sockaddr *) &client, &cl_size);
    printf("Клиент прислал сообщение: %s\n", buffer);

    strcat(buffer, " (From server)");
    sendto(sock_fd, buffer, strlen(buffer), 0, (struct sockaddr *) &client, cl_size);
    printf("Сервер отправил сообщение: %s\n\n", buffer);

    return 0;
}