#include <stdio.h>

int main() {
	int num = 0xA693BF4C;
	printf("Number: %X\n\n", num);
	for (int i = 0; i <= 24; i += 8) {
		printf("%d byte = %X\n", (i / 8) + 1, (num >> i) & 0xFF);
	}
	num &= ~(0xFF << 16);
	num |= 0x1D << 16;
	printf("\nNumber with modifyed 3 byte: %X\n", num);
}
