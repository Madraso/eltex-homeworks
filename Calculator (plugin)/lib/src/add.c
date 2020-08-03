#include "../include/ldfunc.h"

void get_func_inf(struct ldfunc *elem) {
    strcpy(elem->menu_name, "Сложение двух чисел");
    strcpy(elem->lib_name, "add");
}

int add(int a, int b) {
    return a + b;
}