#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <mqueue.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MSG_LEN 256

struct msgbuf {
    long mtype;
    char mtext[MSG_LEN];
};

void exit_with_error(const char *str) {
    perror(str);
    exit(-1);
}

void system_v_messages() {
    key_t key;
    if ((key = ftok("./message_queue", 1)) < 0) {
        exit_with_error("ftok");
    }
    int queue_id;
    if ((queue_id = msgget(key, IPC_CREAT | 0666)) < 0) {
        exit_with_error("msgget");
    }
    struct msgbuf message;
    switch (fork()) {
        case 0: {
            if (msgrcv(queue_id, (void *) &message, MSG_LEN, 0, 0) < 0) {
                exit_with_error("msgrcv");
            }
            printf("Child process got message from parent: %s\n", message.mtext);
            message.mtype = 1;
            strcpy(message.mtext, "Hi!");
            if (msgsnd(queue_id, (void *) &message, strlen(message.mtext) + 1, 0) < 0) {
                exit_with_error("msgsnd");
            }
            exit(0);
        } break;
        default: {
            message.mtype = 1;
            strcpy(message.mtext, "Hello!");
            if (msgsnd(queue_id, (void *) &message, strlen(message.mtext) + 1, 0) < 0) {
                exit_with_error("msgsnd");
            }
            wait(NULL);
            if (msgrcv(queue_id, (void *) &message, MSG_LEN, 0, 0) < 0) {
                exit_with_error("msgrcv");
            }
            printf("Parent process got message from child: %s\n", message.mtext);
            msgctl(queue_id, IPC_RMID, NULL);
        }
    }
}

void posix_messages() {
    mqd_t queue_id;
    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MSG_LEN;
    attr.mq_curmsgs = 0;
    if ((queue_id = mq_open("/posix_queue", O_CREAT | O_RDWR, 0666, &attr)) < 0) {
        exit_with_error("mq_open");
    }
    char message[MSG_LEN];
    switch (fork()) {
        case 0: {
            if (mq_receive(queue_id, message, MSG_LEN, NULL) < 0) {
                exit_with_error("mq_receive");
            }
            printf("Child process got message from parent: %s\n", message);
            strcpy(message, "Hi!");
            if (mq_send(queue_id, message, strlen(message) + 1, 1) < 0) {
                exit_with_error("mq_send");
            }
            exit(0);
        } break;
        default: {
            strcpy(message, "Hello!");
            if (mq_send(queue_id, message, strlen(message) + 1, 1) < 0) {
                exit_with_error("mq_send");
            }
            wait(NULL);
            if (mq_receive(queue_id, message, MSG_LEN, NULL) < 0) {
                exit_with_error("mq_receive");
            }
            printf("Parent process got message from child: %s\n", message);
            mq_close(queue_id);
            mq_unlink("/posix_queue");
        }
    }
}

int main() {
    printf("System V message queue:\n");
    system_v_messages();
    printf("\nPOSIX message queue:\n");
    posix_messages();
}