#include <string.h>

struct ldfunc {
    unsigned char menu_name[256];
    char lib_name[256];
    void *handle;
};