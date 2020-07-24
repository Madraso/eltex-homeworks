#include "../include/Directory.h"

int main() {
    int num;
    char surname[length];
    while (1) {
        system("clear");
        print_menu();
        int key = getch();
        switch (key) {
            case 49: {
                system("clear");
                print_directory();
                getch();
            } break;
            case 50: {
                system("clear");
                add_abonent();
                getch();
            } break;
            case 51: {
                system("clear");
                printf("Введите номер абонента: ");
                scanf("%d", &num);
                clr_input_buf();
                if (edit_abonent(num) == -1) getch(); 
            } break;
            case 52: {
                system("clear");
                printf("Введите номер абонента: ");
                scanf("%d", &num);
                clr_input_buf();
                if (delete_abonent(num) == -1) getch();
            } break;
            case 53: {
                system("clear");
                printf("Введите фамилию абонента: ");
                scanf("%s", surname);
                clr_input_buf();
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