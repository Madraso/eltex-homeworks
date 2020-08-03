#include "../include/ldfunc.h"

void get_func_inf(struct ldfunc *elem) {
    strcpy(elem->menu_name, "Умножение двух чисел");
    strcpy(elem->lib_name, "mul");
}

int mul(int a, int b) {
    return a * b;
}