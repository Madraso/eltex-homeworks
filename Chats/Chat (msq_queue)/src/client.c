#include "../include/client.h"

WINDOW *box_1, *chat, *box_2,
    *message, *box_3, *users;

int client, buf, serv;

int cl_cnt = 0, msg_cnt = 0;

unsigned char cur_username[NICKNAME_LEN];
unsigned char username[CL_CNT][NICKNAME_LEN],
    msg_hist[MSG_CNT][MSG_LEN];

void get_token(unsigned char *mes, char *sep, int tok_num) {
    unsigned char *tmp = strtok(mes, sep);
    for (int i = 0; i < tok_num - 1; i++) {
        tmp = strtok(NULL, sep);
    }
    strcpy(mes, tmp);
}

void check_value(int val, const char *mes) {
    if (val < 0) {
        perror(mes);
        exit_client();
    }
}

void init_windows() {
    initscr();
    curs_set(0);
    start_color();
    init_pair(1, COLOR_CYAN, COLOR_BLACK);

    int max_x, max_y;
    getmaxyx(stdscr, max_y, max_x);

    box_1 = newwin(max_y - 3, max_x - 20, 0, 0);
    chat = derwin(box_1, max_y - 5, max_x - 22, 1, 1);
    box_2 = newwin(max_y - 3, 20, 0, max_x - 20);
    users = derwin(box_2, max_y - 5, 18, 1, 1);
    box_3 = newwin(3, max_x, max_y - 3, 0);
    message = derwin(box_3, 1, max_x - 2, 1, 1);

    keypad(chat, true);
    keypad(users, true);
    scrollok(chat, true);
    scrollok(users, true);

    box(box_1, 0, 0);
    box(box_2, 0, 0);
    box(box_3, 0, 0);

    wrefresh(box_1);
    wrefresh(box_2);
    wrefresh(box_3);
}

void exit_client() {
    delwin(box_1);
    delwin(box_2);
    delwin(box_3);
    delwin(chat);
    delwin(message);
    delwin(users);
    endwin();
    exit(-1);
}

void get_queues_id() {
    key_t buf_key, serv_key;
    buf_key = ftok("./server", CL_CNT + 1);
    check_value(buf_key, "ftok (buf_key)");
    serv_key = ftok("./server", CL_CNT + 2);
    check_value(serv_key, "ftok (serv_key)");
    buf = msgget(buf_key, 0);
    check_value(buf, "msgget (buf)");
    serv = msgget(serv_key, 0);
    check_value(buf, "msgget (serv)");
    
    unsigned char cl_username[NICKNAME_LEN];
    printf("Введите никнейм (не более 15 символов): ");
    fgets(cl_username, NICKNAME_LEN, stdin);
    if (cl_username[strlen(cl_username) - 1] == '\n') {
        cl_username[strlen(cl_username) - 1] = 0;
    }
    strcpy(cur_username, cl_username);

    struct msgbuf srv_msg;

    srv_msg.mtype = 1;
    strcpy(srv_msg.mtext, "new_user ");
    strcat(srv_msg.mtext, cl_username);

    int value = msgsnd(serv, (void *) &srv_msg, sizeof(srv_msg.mtext), 0);
    check_value(value, "msgsnd (serv)");

    value = msgrcv(serv, (void *) &srv_msg, sizeof(srv_msg.mtext), 0, 0);
    check_value(value, "msgrcv (serv)");
    
    if (strstr(srv_msg.mtext, "serv_full") != NULL) {
        printf("Сервер переполнен. Зайдите позднее\n");
        exit_client();
    }
    else client = atoi(srv_msg.mtext);
}

int hdl_serv_msg(unsigned char *msg) {
    if (strstr(msg, "new_user") != NULL) {
        get_token(msg, " ", 2);
        strcpy(username[cl_cnt++], msg);
        return 1;
    }
    if (strstr(msg, "end_users") != NULL) {
        werase(users);
        for (int i = 0; i < cl_cnt; i++) {
            if (strcmp(username[i], cur_username) == 0) {
                wattron(users, COLOR_PAIR(1));
                wprintw(users, "%s\n", username[i]);
                wattroff(users, COLOR_PAIR(1));
            }
            else wprintw(users, "%s\n", username[i]);
        }
        wrefresh(users);
        cl_cnt = 0;
        return 1;
    }
    return 0;
}

void *hdl_chat_wnd(void *args) {
    struct msgbuf cl_msg;
    cl_msg.mtype = 1;

    while (1) {
        int value = msgrcv(client, (void *) &cl_msg, sizeof(cl_msg.mtext), 0, 0);
        check_value(value, "msgrcv (client)");
        if (hdl_serv_msg(cl_msg.mtext) == 1) continue;
        else {
            if (msg_cnt < MSG_CNT - 1) strcpy(msg_hist[msg_cnt++], cl_msg.mtext);
            unsigned char tmp[sizeof(cl_msg.mtext)];
            strcpy(tmp, cl_msg.mtext);
            get_token(tmp, ":", 1);
            if (strcmp(tmp, cur_username) == 0) {
                wattron(chat, COLOR_PAIR(1));
                wprintw(chat, "%s\n", cl_msg.mtext);
                wattroff(chat, COLOR_PAIR(1));
            }
            else wprintw(chat, "%s\n", cl_msg.mtext);
            wrefresh(chat);
        }
    }
}

void *hdl_msg_wnd(void *args) {
    int value;
    unsigned char tmp[MSG_LEN];
    struct msgbuf cl_msg;
    cl_msg.mtype = 1;

    while (1) {
        memset(cl_msg.mtext, 0, sizeof(cl_msg.mtext));
        wscanw(message, "%s", cl_msg.mtext);
        werase(message);
        wrefresh(message);

        if (strcmp(cl_msg.mtext, "") == 0) continue;
        if (strcmp(cl_msg.mtext, "exit") == 0) {
            sprintf(cl_msg.mtext, "exit_user %d", client);

            value = msgsnd(serv, (void *) &cl_msg, sizeof(cl_msg.mtext), 0);
            check_value(value, "msgsnd (serv)");

            exit_client();
        }
        
        strcpy(tmp, cur_username);
        strcat(tmp, ": ");
        strcat(tmp, cl_msg.mtext);
        strcpy(cl_msg.mtext, tmp);

        value = msgsnd(buf, (void *) &cl_msg, sizeof(cl_msg.mtext), 0);
        check_value(value, "msgsnd (buf)");
    }
}

int main() {
    system("clear");
    setlocale(LC_ALL, "");
    get_queues_id();
    init_windows();

    pthread_t hdl_serv_tid, hdl_chat_tid, hdl_msg_tid;
    pthread_create(&hdl_chat_tid, NULL, hdl_chat_wnd, NULL);
    pthread_create(&hdl_msg_tid, NULL, hdl_msg_wnd, NULL);
    pthread_join(hdl_chat_tid, NULL);
    pthread_join(hdl_msg_tid, NULL);
}