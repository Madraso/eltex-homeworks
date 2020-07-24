#ifndef DIRECTORY
#define DIRECTORY

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#define length 20

typedef struct {
    char surname[length];
    char name[length];
    char phone_number[length];
} abonent;

int getch();
void clr_input_buf();
int valid_ptr(void *ptr);

void print_menu();
void print_directory();
void malloc_dir_rec();
void add_abonent();
int edit_abonent(int num);
int delete_abonent(int num);
void find_abonent(char surname[]);

#endif