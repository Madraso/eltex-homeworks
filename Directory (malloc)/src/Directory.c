#include "../include/Directory.h"
#include <malloc.h>

abonent *directory;
int dir_cnt_rec = 0;

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

void clr_input_buf() {    
    while (getchar() != '\n');
}

int valid_ptr(void *ptr) {
    if (ptr == NULL) {
        perror("Error: ");
        return 0;
    }
    else return 1;
}

void print_menu() {
    printf("1. Напечатать справочник\n");
    printf("2. Добавить нового абонента\n");
    printf("3. Изменить существующего абонента\n");
    printf("4. Удалить абонента\n");
    printf("5. Поиск абонента по фамилии\n");
    printf("6. Выход из программы\n");
}

void print_directory() {
    for (int i = 0; i < dir_cnt_rec; i++) {
        printf("%d. %s %s, %s\n", i + 1, directory[i].surname,
        directory[i].name, directory[i].phone_number);
    }
    if (dir_cnt_rec == 0) printf("Справочник пуст\n");
}

void malloc_dir_rec() {
    if (!directory) {
        directory = (abonent *) malloc(dir_cnt_rec * sizeof(abonent));
        if (!valid_ptr(directory)) exit(-1);
    }
    else {
        if (dir_cnt_rec == 0) {
            memset(directory[0].surname, '\0', length);
            memset(directory[0].name, '\0', length);
            memset(directory[0].phone_number, '\0', length);
        }
        else {
            directory = realloc(directory, dir_cnt_rec * sizeof(abonent));
            if (!valid_ptr(directory)) exit(-1);
        }
    }
}

void add_abonent() {
    malloc_dir_rec(dir_cnt_rec++);
    printf("Введите фамилию: ");
    scanf("%s", directory[dir_cnt_rec - 1].surname);
    printf("Введите имя: ");
    scanf("%s", directory[dir_cnt_rec - 1].name);
    printf("Введите телефон: ");
    scanf("%s", directory[dir_cnt_rec - 1].phone_number);
}

int edit_abonent(int num) {
    if (num <= 0 || num > dir_cnt_rec) {
        printf("Введен неверный номер абонента\n");
        return -1;
    }
    else {
        printf("Введите фамилию: ");
        scanf("%s", directory[num - 1].surname);
        printf("Введите имя: ");
        scanf("%s", directory[num - 1].name);
        printf("Введите телефон: ");
        scanf("%s", directory[num - 1].phone_number);
        return 0;
    }
}

int delete_abonent(int num) {
    if (num <= 0 || num > dir_cnt_rec) {
        printf("Введен неверный номер абонента\n");
        return -1;
    }
    for (int i = num - 1; i < dir_cnt_rec - 1; i++) {
        strcpy(directory[i].surname, directory[i + 1].surname);
        strcpy(directory[i].name, directory[i + 1].name);
        strcpy(directory[i].phone_number, directory[i + 1].phone_number);
    }
    malloc_dir_rec(dir_cnt_rec--);
    return 0;
}

void find_abonent(char surname[]) {
    int counter = 0;
    for (int i = 0; i < dir_cnt_rec; i++) {
        if (strcmp(directory[i].surname, surname) == 0) {
            printf("Найден абонент - %s %s, %s\n", directory [i].surname,
            directory[i].name, directory[i].phone_number);
            counter++;
        }
    }
    if (counter == 0) printf("Абонентов с фамилией %s не найдено\n", surname);
}