#include "../include/struct.h"
#include "../include/server.h"

int client[CL_CNT], buf, serv;

int cl_cnt = 0, msg_cnt = 0;

unsigned char nickname[CL_CNT][NICKNAME_LEN],
    msg_hist[MSG_CNT][MSG_LEN];

int getch() {
    int ch;
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    tcgetattr(STDIN_FILENO, &newt);
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
}

void get_token(unsigned char *mes) {
    unsigned char *tmp = strtok(mes, " ");
    tmp = strtok(NULL, " ");
    strcpy(mes, tmp);
}

void arr_shift(char *arr, int left, int right) {
    if (strcmp(arr, "client") == 0) {
        for (int i = left; i < right; i++) {
            client[i] = client[i + 1];
        }
    }
    if (strcmp(arr, "nickname") == 0) {
        for (int i = left; i < right; i++) {
            strcpy(nickname[i], nickname[i + 1]);
        }
    }
}

void check_value(int val, const char *mes) {
    if (val < 0) {
        perror(mes);
        for (int i = 0; i < cl_cnt; i++) msgctl(client[i], IPC_RMID, NULL);
        if (buf != -1) msgctl(buf, IPC_RMID, NULL);
        if (serv != -1) msgctl(serv, IPC_RMID, NULL);
        exit(-1);
    }
}

void snd_nicknames() {
    int value;
    struct msgbuf message;
    message.mtype = 1;
    for (int i = 0; i < cl_cnt; i++) {
        for (int j = 0; j < cl_cnt; j++) {
            strcpy(message.mtext, "new_user ");
            strcat(message.mtext, nickname[j]);
            value = msgsnd(client[i], (void *) &message, sizeof(message.mtext), 0);
            check_value(value, "msgsnd (client)");
        }
        strcpy(message.mtext, "end_users");
        value = msgsnd(client[i], (void *) &message, sizeof(message.mtext), 0);
        check_value(value, "msgsnd (client)");
    }
}

void snd_msg_hist() {
    int value;
    struct msgbuf message;
    message.mtype = 1;
    for (int i = 0; i < msg_cnt; i++) {
        strcpy(message.mtext, msg_hist[i]);
        value = msgsnd(client[cl_cnt - 1], (void *) &message, sizeof(message.mtext), 0);
        check_value(value, "msgsnd (client)");
    }
}

void *hdl_serv_keys(void *args) {
    while (1) {
        int key = getch();
        switch (key) {
            case 27: {
                printf("Завершение работы сервера...\n");
                for (int i = 0; i < cl_cnt; i++) msgctl(client[i], IPC_RMID, NULL);
                if (buf != -1) msgctl(buf, IPC_RMID, NULL);
                if (serv != -1) msgctl(serv, IPC_RMID, NULL);
                exit(0);
            } break;
        }
    }
}

void *hdl_serv_queue(void *args) {
    struct msgbuf message;
    message.mtype = 1;
    while (1) {
        int value = msgrcv(serv, (void *) &message, sizeof(message.mtext), 0, 0);
        check_value(value, "msgrcv (serv)");
        if (strstr(message.mtext, "new_user") != NULL) {
            if (cl_cnt != CL_CNT) {
                unsigned char tmp[sizeof(message.mtext)];
                strcpy(tmp, message.mtext);
                get_token(tmp);
                strcpy(nickname[cl_cnt], tmp);

                key_t cl_key = ftok("./server", cl_cnt);
                check_value(cl_key, "ftok (client)");
                client[cl_cnt] = msgget(cl_key, IPC_CREAT | 0666);
                check_value(client[cl_cnt], "msgget (client)");

                sprintf(message.mtext, "%d", client[cl_cnt]);
                value = msgsnd(serv, (void *) &message, sizeof(message.mtext), 0);
                check_value(value, "msgsnd (serv)");

                printf("Клиент с идентификатором %d подключился к серверу\n", client[cl_cnt]);

                cl_cnt++;

                sleep(1);
                snd_nicknames();
                snd_msg_hist();
            }
            else {
                strcpy(message.mtext, "serv_full");
                value = msgsnd(serv, (void *) &message, sizeof(message.mtext), 0);
                check_value(value, "msgsnd (serv)");
            }
        }
        if (strstr(message.mtext, "exit_user") != NULL) {
            get_token(message.mtext);
            int id = atoi(message.mtext);
            for (int i = 0; i < cl_cnt; i++) {
                if (client[i] == id) {
                    msgctl(client[i], IPC_RMID, NULL);
                    arr_shift("client", i, cl_cnt - 1);
                    arr_shift("nickname", i, cl_cnt - 1);
                    printf("Клиент с идентификатором %d отключился от сервера\n", id);
                    cl_cnt--;
                    snd_nicknames();
                    break;
                }
            }
        }
    }
}

int main() {
    system("clear");
    setlocale(LC_ALL, "");
    key_t buf_key, serv_key;
    buf_key = ftok("./server", CL_CNT + 1);
    check_value(buf_key, "ftok (buf_key)");
    serv_key = ftok("./server", CL_CNT + 2);
    check_value(serv_key, "ftok (serv_key)");
    buf = msgget(buf_key, IPC_CREAT | 0666);
    check_value(buf, "msgget (buf)");
    printf("Создана транзитная очередь\n");
    serv = msgget(serv_key, IPC_CREAT | 0666);
    check_value(buf, "msgget (serv)");
    printf("Создана служебная очередь\n");

    pthread_t hdl_keys_tid, hdl_serv_tid;
    pthread_create(&hdl_keys_tid, NULL, hdl_serv_keys, NULL);
    pthread_create(&hdl_serv_tid, NULL, hdl_serv_queue, NULL);

    struct msgbuf message;
    while (1) {
        int value = msgrcv(buf, (void *) &message, sizeof(message.mtext), 0, 0);
        check_value(value, "msgrcv (buf)");
        if (msg_cnt < MSG_CNT - 1) strcpy(msg_hist[msg_cnt++], message.mtext);
        for (int i = 0; i < cl_cnt; i++) {
            value = msgsnd(client[i], (void *) &message, sizeof(message.mtext), 0);
            check_value(value, "msgsnd (client)");
        }
    }
}