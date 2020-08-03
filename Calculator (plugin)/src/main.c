#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <dirent.h>
#include <malloc.h>
#include <dlfcn.h>
#include "../lib/include/ldfunc.h"

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

void clear_buffer() {    
    while (getchar() != '\n');
}

void input_nums(int *a, int *b) {
    system("clear");
    printf("Введите первое число: ");
    while (scanf("%d", a) != 1) {
        system("clear");
        printf("Введите первое число: ");
        clear_buffer();
    }
    printf("Введите второе число: ");
    while (scanf("%d", b) != 1) { 
        system("clear");
        printf("Введите второе число: ");
        clear_buffer();
    }
    clear_buffer();
}

void menu(struct ldfunc *functions, int size) {
    system("clear");
    int i;
    for (i = 0; i < size; i++) {
        printf("%d. %s\n", i + 1, functions[i].menu_name);
    }
    printf("%d. Выход из программы\n", i + 1);
}

struct ldfunc* load_libraries(int *size) {
    struct dirent **namelist;
    char *error, dir_path[FILENAME_MAX];
    getcwd(dir_path, FILENAME_MAX);
    strcat(dir_path, "/plugin");
    int lib_cnt = scandir(dir_path, &namelist, NULL, alphasort);
    if (lib_cnt < 0) {
        perror("Plugin directory: ");
        exit(EXIT_FAILURE);
    }
    struct ldfunc *functions = (struct ldfunc *) malloc((lib_cnt - 2) * sizeof(struct ldfunc));
    *size = lib_cnt - 2;
    for (int i = 2; i < lib_cnt; i++) {
        void *handle = dlopen(namelist[i]->d_name, RTLD_LAZY);
        if (!handle) {
            error = dlerror();
            write(2, error, strlen(error));
            exit(EXIT_FAILURE);
        }
        void (*func_inf)(struct ldfunc *);
        func_inf = dlsym(handle, "get_func_inf");
        error = dlerror();
        if (error != NULL) {
            write(2, error, strlen(error));
            exit(EXIT_FAILURE);
        }
        functions[i - 2].handle = handle;
        func_inf(&functions[i - 2]);
    }
    for (int i = 0; i < lib_cnt; i++) free(namelist[i]);
    free(namelist);
    return functions;
}

int main() {
    char ch;
    int digit, a, b, st_size;
    struct ldfunc *functions = load_libraries(&st_size);
    while (1) {
        menu(functions, st_size);
        ch = getch();
        digit = ch - '0';
        if (digit < 1 || digit > st_size + 1) {}
        else {
            if (digit == st_size + 1) {
                for (int i = 0; i < st_size; i++) dlclose(functions[i].handle);
                free(functions);
                system("clear");
                exit(EXIT_SUCCESS);
            }
            else {
                input_nums(&a, &b);
                int (*func)(int, int) = dlsym(functions[digit - 1].handle, functions[digit - 1].lib_name);
                printf("Результат: %d\n", func(a, b));
                getch();
            }
        }
    }
}