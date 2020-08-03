#include "../include/ldfunc.h"

void get_func_inf(struct ldfunc *elem) {
    strcpy(elem->menu_name, "Разность двух чисел");
    strcpy(elem->lib_name, "diff");
}

int diff(int a, int b) {
    return a - b;
}