#include "../include/struct.h"
#include "../include/server.h"

pthread_mutex_t serv_mutex;
pthread_cond_t serv_cond;

struct chat *chat_inf;

int shm_fd;

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

void *hdl_serv_keys(void *args) {
    while (1) {
        int key = getch();
        switch (key) {
            case 27: {
                printf("Завершение работы сервера...\n");
                munmap(chat_inf, sizeof(chat_inf));
                close(shm_fd);
                shm_unlink(SHM_NAME);
                sem_destroy(&chat_inf->sem_mutex);
                sem_destroy(&chat_inf->usr_cnt);
                sem_destroy(&chat_inf->msg_cnt);
                pthread_mutex_destroy(&chat_inf->pth_mutex);
                pthread_cond_destroy(&chat_inf->cond_chat);
                pthread_cond_destroy(&chat_inf->cond_users);
                exit(0);
            } break;
        }
    }
}

void check_value(int val, const char *mes) {
    if (val < 0) {
        perror(mes);
        exit(-1);
    }
}

int main() {
    setlocale(LC_ALL, "");
    system("clear");
    shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    check_value(shm_fd, "shm_open");
    int trunc_val = ftruncate(shm_fd, sizeof(struct chat));
    check_value(trunc_val, "ftruncate");
    chat_inf = (struct chat *) mmap(NULL, sizeof(struct chat), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (chat_inf == MAP_FAILED) check_value(-1, "mmap");
    printf("Создана разделяемая память\n");
    int sem_init_val = sem_init(&chat_inf->sem_mutex, 1, 1);
    check_value(sem_init_val, "sem_init (mutex)");
    sem_init_val = sem_init(&chat_inf->usr_cnt, 1, 0);
    check_value(sem_init_val, "sem_init (usr_cnt)");
    sem_init_val = sem_init(&chat_inf->msg_cnt, 1, 0);
    check_value(sem_init_val, "sem_init (msg_cnt)");
    printf("Созданы семафоры\n");

    pthread_mutexattr_t mutex_attr;
    pthread_condattr_t cond_chat_attr, cond_users_attr;

    pthread_mutexattr_init(&mutex_attr);
    pthread_mutexattr_setpshared(&mutex_attr, PTHREAD_PROCESS_SHARED);
    pthread_mutex_init(&chat_inf->pth_mutex, &mutex_attr);

    pthread_condattr_init(&cond_chat_attr);
    pthread_condattr_setpshared(&cond_chat_attr, PTHREAD_PROCESS_SHARED);
    pthread_cond_init(&chat_inf->cond_chat, &cond_chat_attr);

    pthread_condattr_init(&cond_users_attr);
    pthread_condattr_setpshared(&cond_users_attr, PTHREAD_PROCESS_SHARED);
    pthread_cond_init(&chat_inf->cond_users, &cond_users_attr);

    pthread_mutex_init(&serv_mutex, NULL);
    pthread_cond_init(&serv_cond, NULL);

    pthread_t hdl_keys_tid;
    pthread_create(&hdl_keys_tid, NULL, hdl_serv_keys, NULL);

    pthread_cond_wait(&serv_cond, &serv_mutex);
}