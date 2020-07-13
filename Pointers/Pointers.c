#include <stdio.h>

int main() {
    int num = 2439719549, i = 0;
    unsigned char *ptr = &num;
    printf("Number: 0x%X\n\n", num);
    while (1) {
        printf("%d byte = 0x%X\n", i + 1, *ptr);
        if (i == sizeof(int) - 1) break;
        ptr++, i++;
    }
    printf("\n");
    ptr -= 1;
    *ptr = 0xFF;
    printf("Number with modified 3rd byte: 0x%X\n", num);
}