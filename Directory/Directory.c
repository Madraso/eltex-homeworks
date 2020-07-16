#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

const int N = 10;

struct abonent {
    char surname[20];
    char name[20];
    char phone_number[12];
} directory[10];

void init() {
    for (int i = 0; i < N; i++) {
        memset(&directory[i], '\0', sizeof(directory[i]));
    }
}

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

void menu() {
    printf("1. Напечатать справочник\n");
    printf("2. Добавить нового абонента\n");
    printf("3. Изменить существующего абонента\n");
    printf("4. Удалить абонента\n");
    printf("5. Поиск абонента по фамилии\n");
    printf("6. Выход из программы\n");
}

void print_directory() {
    int count = 0;
    for (int i = 0; i < N; i++) {
        if (directory[i].surname[0] != '\0') {
            printf("%d. %s %s, %s\n", i + 1, directory[i].surname,
            directory[i].name, directory[i].phone_number);
        }
        else count++;
    }
    if (count == N) printf("Справочник пуст\n");
}

int add_abonent() {
    for (int i = 0; i < N; i++) {
        if (directory[i].surname[0] == '\0') {
            printf("Введите фамилию: ");
            scanf("%s", &directory[i].surname);
            printf("Введите имя: ");
            scanf("%s", &directory[i].name);
            printf("Введите телефон: ");
            scanf("%s", &directory[i].phone_number);
            return 0;
        }
    }
    printf("В справочнике нет места\n");
    return -1;
}

int edit_abonent(int num) {
    if (num <= 0 || num > 10) {
        printf("Введен неверный номер абонента\n");
        return -1;
    }
    if (directory[num - 1].surname[0] != '\0') {
        printf("Введите фамилию: ");
        scanf("%s", &directory[num - 1].surname);
        printf("Введите имя: ");
        scanf("%s", &directory[num - 1].name);
        printf("Введите телефон: ");
        scanf("%s", &directory[num - 1].phone_number);
        return 0;
    }
    else {
        printf("Абонента с номером %d нет в справочнике\n", num);
        return -1;
    }
}

int delete_abonent(int num) {
    if (num <= 0 || num > 10) {
        printf("Введен неверный номер абонента\n");
        return -1;
    }
    if (directory[num - 1].surname[0] != '\0') {
        memset(&directory[num - 1], '\0', sizeof(directory[num - 1]));
    }
    else {
        printf("Абонента с номером %d нет в справочнике\n", num);
        return -1;
    }
    return 0;
}

void find_abonent(char surname[]) {
    for (int i = 0; i < N; i++) {
        if (strcmp(directory[i].surname, surname) == 0) {
            printf("Абонент найден - %s %s, %s\n", directory [i].surname,
            directory[i].name, directory[i].phone_number);
            return;
        }
    }
    printf("Абонент с фамилией %s не найден\n", surname);
}

int main() {
    int num;
    char surname[20];
    init();
    while (1) {
        system("clear");
        menu();
        int key = getch();
        switch (key) {
            case 49: {
                system("clear");
                print_directory();
                getch();
            } break;
            case 50: {
                system("clear");
                if (add_abonent() == -1) getch();
            } break;
            case 51: {
                system("clear");
                printf("Введите номер абонента: ");
                scanf("%d", &num);
                clear_buffer();
                if (edit_abonent(num) == -1) getch(); 
            } break;
            case 52: {
                system("clear");
                printf("Введите номер абонента: ");
                scanf("%d", &num);
                clear_buffer();
                if (delete_abonent(num) == -1) getch();
            } break;
            case 53: {
                system("clear");
                printf("Введите фамилию абонента: ");
                scanf("%s", &surname);
                clear_buffer();
                find_abonent(surname);
                getch();
            } break;
            case 54: {
                system("clear");
                exit(0);
            } break;
        }
    }
    return 0;
}