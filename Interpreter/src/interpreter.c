#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <pwd.h>
#include <unistd.h>

#define STR_CNT 20
#define STR_SIZE 256

void sep_str(char *cmd, char *sep, char *tokens[STR_CNT], int *size) {
    char *cmd_token = strtok(cmd, sep);
    int i = 0;
    while (cmd_token != NULL) {
        tokens[i++] = cmd_token;
        cmd_token = strtok(NULL, sep);
    }
    tokens[i] = NULL;
    *size = i;
}

void get_change_desc_mode(char *mode, int index, int conveyor_size) {
    if (conveyor_size == 1) strcpy(mode, "only");
    else if (index == 0) strcpy(mode, "first");
    else if (index == conveyor_size - 1) strcpy(mode, "last");
    else strcpy(mode, "middle");
}

void change_desc_child(char *mode, int fd_old[], int fd_new[]) {
    if (strcmp(mode, "middle") == 0) {
        dup2(fd_old[STDIN_FILENO], STDIN_FILENO);
        close(fd_old[0]);
        close(fd_old[1]);
        close(fd_new[0]);
        dup2(fd_new[STDOUT_FILENO], STDOUT_FILENO);
        close(fd_new[1]);
    }
    else if (strcmp(mode, "first") == 0) {
        close(fd_new[0]);
        dup2(fd_new[STDOUT_FILENO], STDOUT_FILENO);
        close(fd_new[1]);
    }
    else if (strcmp(mode, "last") == 0) {
        dup2(fd_old[STDIN_FILENO], STDIN_FILENO);
        close(fd_old[0]);
        close(fd_old[1]);
    }
}

void change_desc_parent(char *mode, int fd_old[], int fd_new[]) {
    if (strcmp(mode, "middle") == 0) {
        close(fd_old[0]);
        close(fd_old[1]);
        fd_old[0] = fd_new[0];
        fd_old[1] = fd_new[1];
    }
    else if (strcmp(mode, "first") == 0) {
        fd_old[0] = fd_new[0];
        fd_old[1] = fd_new[1];
    }
    else if (strcmp(mode, "last") == 0) {
        close(fd_old[0]);
        close(fd_old[1]);
    }
}

int main() {
    int fd_old[2], fd_new[2], conveyor_size, conv_cmd_size;
    char cmd[STR_SIZE + 1], mode[10], pwd[STR_SIZE],
        *conveyor[STR_CNT], *conv_cmd[STR_CNT];
    struct passwd *user_inf = getpwuid(getuid());

    system("clear");

    while (1) {
        getcwd(pwd, STR_SIZE);
        printf("%s:%s$ ", user_inf->pw_name, pwd);
        fgets(cmd, STR_SIZE, stdin);
        cmd[strlen(cmd) - 1] = 0;
        sep_str(cmd, "|", conveyor, &conveyor_size);
        for (int i = 0; i < conveyor_size; i++) {
            pipe(fd_new);
            sep_str(conveyor[i], " ", conv_cmd, &conv_cmd_size);
            if (fork() == 0) {
                get_change_desc_mode(mode, i, conveyor_size);
                change_desc_child(mode, fd_old, fd_new);
                if (execvp(conv_cmd[0], conv_cmd) < 0) {
                    perror("execvp");
                    exit(-1);
                }
            }
            else {
                get_change_desc_mode(mode, i, conveyor_size);
                change_desc_parent(mode, fd_old, fd_new);
                wait(NULL);
            }
        }
    }
    return 0;
}