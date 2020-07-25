#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

void wrt_buf_in_file(char *filename, char *buffer, int buf_size) {
    int fd = open(filename, O_CREAT | O_RDWR, 0666);
    if (fd < 0) {
        perror("Error");
        exit(-1);
    }
    write(fd, buffer, buf_size);
    close(fd);
}

void reverse_read_file(char *filename) {
    int fd = open(filename, O_RDONLY);
    if (fd < 0) {
        perror("Error");
        exit(-1);
    }
    char symbol;
    int pos = lseek(fd, -1, SEEK_END);
    while (read(fd, &symbol, 1)) {
        printf("%c", symbol);
        if (pos == 0) break;
        else pos = lseek(fd, -2, SEEK_CUR);
    }
    printf("\n");
}

int main(int argc, char *argv[]) {
    char buffer[FILENAME_MAX] = "Hello world!";
    char filepath[FILENAME_MAX];
    getcwd(filepath, sizeof(filepath));
    strcat(filepath, "/text.txt");
    wrt_buf_in_file(filepath, buffer, strlen(buffer));
    reverse_read_file(filepath);
}