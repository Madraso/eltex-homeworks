#include <stdio.h>

const int N = 5;

// Вывести квадратную матрицу по заданному N
void task_1() {
    int array[N][N];
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            array[i][j] = i * N + (j + 1);
        }
    }
    printf("Task 1:\n");
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            printf("%d\t", array[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

// Вывести заданный массив размером N в обратном порядке
void task_2() {
    int array[N];
    for (int i = 0; i < N; i++) array[i] = i + 1;
    printf("Task 2:\n");
    for (int i = N - 1; i >= 0; i--) printf("%d\t", array[i]);
    printf("\n\n");
}

// Заполнить верхний треугольник матрицы 1, а нижний - 0
void task_3() {
    int array[N][N];
    for (int i = 0; i < N; i++) {
        for (int j = i; j < N; j++) {
            array[i][j] = 1;
        }
    }
    for (int i = 1; i < N; i++) {
        for (int j = 0; j < i; j++) {
            array[i][j] = 0;
        }
    }
    printf("Task 3:\n");
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            printf("%d\t", array[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

// Заполнить матрицу числами от 1 до N^2 улиткой
void task_4() {
    int array[N][N];
    int border_1 = 0, border_2 = N, num = 1;
    while (border_1 < border_2) {
        for (int i = border_1; i < border_2; i++) {
            array[border_1][i] = num++;
        }
        for (int i = border_1 + 1; i < border_2; i++) {
            array[i][border_2 - 1] = num++;
        }
        for (int i = border_2 - 2; i >= border_1; i--) {
            array[border_2 - 1][i] = num++;
        }
        for (int i = border_2 - 2; i > border_1; i--) {
            array[i][border_1] = num++;
        }
        border_1++, border_2--;
    }
    printf("Task 4:\n");
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            printf("%d\t", array[i][j]);
        }
        printf("\n");
    }
}

int main() {
    task_1();
    task_2();
    task_3();
    task_4();
    return 0;
}