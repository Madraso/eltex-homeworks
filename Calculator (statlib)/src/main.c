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

void clr_input_buf() {    
    while (getchar() != '\n');
}

int main() {
    int ch, a, b;
    while (1) {
        menu();
        ch = getch();
        switch (ch) {
            case 49: {
                input_nums(&a, &b);
                clr_input_buf();
                printf("Результат: %d\n", add(a, b));
                getch();
            } break;
            case 50: {
                input_nums(&a, &b);
                clr_input_buf();
                printf("Результат: %d\n", diff(a, b));
                getch();
            } break;
            case 51: {
                input_nums(&a, &b);
                clr_input_buf();
                printf("Результат: %d\n", mul(a, b));
                getch();
            } break;
            case 52: {
                input_nums(&a, &b);
                clr_input_buf();
                printf("Результат: %d\n", divide(a, b));
                getch();
            } break;
            case 53: {
                system("clear");
                exit(EXIT_SUCCESS);
            } break;
        }
    }
}