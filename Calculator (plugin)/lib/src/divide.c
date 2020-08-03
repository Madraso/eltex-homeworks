#include "../include/ldfunc.h"

void get_func_inf(struct ldfunc *elem) {
    strcpy(elem->menu_name, "Деление двух чисел");
    strcpy(elem->lib_name, "divide");
}

int divide(int a, int b) {
    if (b == 0) return 0;
    else return a / b;
}