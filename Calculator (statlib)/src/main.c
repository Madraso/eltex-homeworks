#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include "../lib/include/calc.h"

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

void menu() {
    system("clear");
    printf("1. Сложение двух чисел\n");
    printf("2. Разность двух чисел\n");
    printf("3. Умножение двух чисел\n");
    printf("4. Деление двух чисел\n");
    printf("5. Выход из программы\n");
}

int main() {
    int ch, a, b;
    while (1) {
        menu();
        ch = getch();
        switch (ch) {
            case 49: {
                input_nums(&a, &b);
                printf("Результат: %d + %d = %d\n", a, b, add(a, b));
                getch();
            } break;
            case 50: {
                input_nums(&a, &b);
                printf("Результат: %d - %d = %d\n", a, b, diff(a, b));
                getch();
            } break;
            case 51: {
                input_nums(&a, &b);
                printf("Результат: %d * %d = %d\n", a, b, mul(a, b));
                getch();
            } break;
            case 52: {
                input_nums(&a, &b);
                if (b == 0) printf("Деление на ноль невозможно!\n");
                else printf("Результат: %d / %d = %d\n", a, b, divide(a, b));
                getch();
            } break;
            case 53: {
                system("clear");
                exit(EXIT_SUCCESS);
            } break;
        }
    }
}