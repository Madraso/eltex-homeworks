#include "../include/struct.h"
#include "../include/client.h"

WINDOW *box_1, *chat, *box_2,
    *message, *box_3, *users;

struct chat *chat_inf;

unsigned char username[NICKNAME_LEN];
int shm_fd, usr_cnt, msg_cnt, exit_flg;

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
    sem_wait(&chat_inf->sem_mutex);

    sem_getvalue(&chat_inf->usr_cnt, &usr_cnt);
    for (int i = 0; i < usr_cnt; i++) {
        if (strcmp(chat_inf->user[i], username) == 0) {
            for (int j = i; j < usr_cnt - 1; j++) {
                strcpy(chat_inf->user[j], chat_inf->user[j + 1]);
            }
            sem_wait(&chat_inf->usr_cnt);
            pthread_cond_broadcast(&chat_inf->cond_users);
            break;
        }
    }

    sem_post(&chat_inf->sem_mutex);
    
    munmap(chat_inf, sizeof(chat_inf));
    close(shm_fd);
    delwin(box_1);
    delwin(box_2);
    delwin(box_3);
    delwin(chat);
    delwin(message);
    delwin(users);
    endwin();
    exit(0);
}

void shmem_attach() {
    shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);
    check_value(shm_fd, "shm_open");
    chat_inf = (struct chat *) mmap(NULL, sizeof(struct chat), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (chat_inf == MAP_FAILED) check_value(-1, "mmap");

    printf("Введите никнейм (не более 15 символов): ");
    fgets(username, NICKNAME_LEN, stdin);
    if (username[strlen(username) - 1] == '\n') {
        username[strlen(username) - 1] = 0;
    }
    
    sem_wait(&chat_inf->sem_mutex);

    sem_getvalue(&chat_inf->usr_cnt, &usr_cnt);
    if (usr_cnt < CL_CNT - 1) {
        strcpy(chat_inf->user[usr_cnt], username);
        sem_post(&chat_inf->usr_cnt);
        sem_post(&chat_inf->sem_mutex);
    }
    else {
        printf("Сервер переполнен. Зайдите позднее\n");
        sem_post(&chat_inf->sem_mutex);
        exit(-1);
    }
}

void *hdl_chat_wnd(void *args) {
    unsigned char msg_username[MSG_LEN];

    while (1) {
        pthread_mutex_lock(&chat_inf->pth_mutex);
        pthread_cond_wait(&chat_inf->cond_chat, &chat_inf->pth_mutex);
        sem_getvalue(&chat_inf->msg_cnt, &msg_cnt);
        werase(chat);
        for (int i = 0; i < msg_cnt; i++) {
            strcpy(msg_username, chat_inf->msg[i]);
            get_token(msg_username, ":", 1);
            if (strcmp(msg_username, username) == 0) {
                wattron(chat, COLOR_PAIR(1));
                wprintw(chat, "%s\n", chat_inf->msg[i]);
                wattroff(chat, COLOR_PAIR(1));
            }
            else wprintw(chat, "%s\n", chat_inf->msg[i]);
        }
        wrefresh(chat);
        pthread_mutex_unlock(&chat_inf->pth_mutex);
        if (exit_flg) pthread_exit(NULL);
    }
}

void *hdl_users_wnd(void *args) {
    while (1) {
        pthread_mutex_lock(&chat_inf->pth_mutex);
        pthread_cond_wait(&chat_inf->cond_users, &chat_inf->pth_mutex);
        sem_getvalue(&chat_inf->usr_cnt, &usr_cnt);
        werase(users);
        for (int i = 0; i < usr_cnt; i++) {
            if (strcmp(username, chat_inf->user[i]) == 0) {
                wattron(users, COLOR_PAIR(1));
                wprintw(users, "%s\n", chat_inf->user[i]);
                wattroff(users, COLOR_PAIR(1));
            }
            else wprintw(users, "%s\n", chat_inf->user[i]);
        }
        wrefresh(users);
        pthread_mutex_unlock(&chat_inf->pth_mutex);
        if (exit_flg) pthread_exit(NULL);
    }
}

void *hdl_msg_wnd(void *args) {
    unsigned char msg[MSG_LEN],
        msg_with_usr[MSG_LEN + NICKNAME_LEN + 2];

    while (1) {
        werase(message);
        memset(msg, 0, sizeof(msg));

        wscanw(message, "%s", msg);

        if (strlen(msg) == 0) continue;
        if (strcmp(msg, "exit") == 0) {
            exit_flg = 1;
            pthread_cond_broadcast(&chat_inf->cond_chat);
            pthread_cond_broadcast(&chat_inf->cond_users);
            pthread_exit(NULL);
        }

        sem_wait(&chat_inf->sem_mutex);

        sprintf(msg_with_usr, "%s: %s", username, msg);
        sem_getvalue(&chat_inf->msg_cnt, &msg_cnt);
        if (msg_cnt == MSG_CNT - 1) {
            msg_cnt = 0;
            for (int i = 0; i < MSG_CNT - 1; i++) sem_wait(&chat_inf->msg_cnt);
        }
        strcpy(chat_inf->msg[msg_cnt], msg_with_usr);
        sem_post(&chat_inf->msg_cnt);

        sem_post(&chat_inf->sem_mutex);

        pthread_mutex_lock(&chat_inf->pth_mutex);
        pthread_cond_broadcast(&chat_inf->cond_chat);
        pthread_mutex_unlock(&chat_inf->pth_mutex);
    }
}

int main() {
    setlocale(LC_ALL, "");
    system("clear");

    shmem_attach();
    init_windows();
    
    pthread_t hdl_chat_tid, hdl_users_tid, hdl_msg_tid;
    
    pthread_create(&hdl_chat_tid, NULL, hdl_chat_wnd, NULL);
    pthread_create(&hdl_users_tid, NULL, hdl_users_wnd, NULL);
    pthread_create(&hdl_msg_tid, NULL, hdl_msg_wnd, NULL);

    sleep(1);

    pthread_mutex_lock(&chat_inf->pth_mutex);
    pthread_cond_broadcast(&chat_inf->cond_users);
    pthread_cond_broadcast(&chat_inf->cond_chat);
    pthread_mutex_unlock(&chat_inf->pth_mutex);

    pthread_join(hdl_chat_tid, NULL);
    pthread_join(hdl_users_tid, NULL);
    pthread_join(hdl_msg_tid, NULL);

    exit_client();
}