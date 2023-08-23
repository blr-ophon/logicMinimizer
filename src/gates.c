#include "gates.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_INPUT_LEN 100
#define MAX_EXPR_LEN 100

char matrix[MAX_INPUT_LEN * MAX_EXPR_LEN];

typedef enum {
    RIGHT,
    DOWN
} Direction;

void matrixPut(char *matrix, int h, int w, int x, int y, const char *s, Direction dir) {
    if (x >= w || y >= h) {
        printf("matrixPut: point (%d, %d) out of matrix bounds!\n", x, y);
        return;
    }

    char *ins = matrix + (y * w) + x;
    char *end = matrix + (h * w);

    while (*s) {
        if (ins >= end) break;
        *ins = *s;

        if (dir == RIGHT) {
            ins++;
        } else {
            ins += w;
        }
        s++;
    }
}

void matrixPrint(char *matrix, int h, int w) {
    for (int i = 0; i < h * w; i++) {
        putchar(matrix[i]);
        if ((i + 1) % w == 0) putchar('\n');
    }
}

void clearMatrix(char *matrix, int h, int w) {
    for (int i = 0; i < h * w; i++) {
        matrix[i] = ' ';
    }
}

void repeat(char *s, int n, char *result) {
    for (int i = 0; i < n; i++) {
        strcat(result, s);
    }
}

int main(int argc, char *argv[]) {
    char expression[MAX_EXPR_LEN];

    if (argc == 1) {
        printf("Enter expression:\n");
        fgets(expression, MAX_EXPR_LEN, stdin);
        expression[strcspn(expression, "\n")] = '\0'; // Remove trailing newline
    } else {
        strncpy(expression, argv[1], MAX_EXPR_LEN);
        expression[MAX_EXPR_LEN - 1] = '\0'; // Ensure null-termination
    }

    char buf[MAX_EXPR_LEN]; // Temporary buffer for string manipulations
    char *expr = buf; // Use buf as the expression

    // Remove spaces from the expression
    int i, j = 0;
    for (i = 0; expression[i]; i++) {
        if (expression[i] != ' ') {
            expr[j++] = expression[i];
        }
    }
    expr[j] = '\0';

    int height = 0; // Calculate height based on expression
    int width = 0;  // Calculate width based on expression

    // Rest of the code (transferred from C++ to C)...

    // Clear the matrix
    clearMatrix(matrix, height, width);

    // Rest of the code (transferred from C++ to C)...

    return 0;
}
