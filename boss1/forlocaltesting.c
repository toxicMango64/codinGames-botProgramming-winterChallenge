#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ROWS 100
#define MAX_COLS 100

void readMapFromFile(const char *filename, char map[MAX_ROWS][MAX_COLS], int *rows, int *cols) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    *rows = 0;
    size_t len = 0;
    while (fgets(map[*rows], MAX_COLS, file) != NULL) {
        // Remove newline character if present
        size_t len = strlen(map[*rows]);
        if (len > 0 && map[*rows][len - 1] == '\n') {
            map[*rows][len - 1] = '\0';
        }
        (*rows)++;
    }
    *cols = len; // Assuming all rows have the same number of columns

    fclose(file);
}

void printMap(char map[MAX_ROWS][MAX_COLS], int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        printf("%s\n", map[i]);
    }
}

int main() {
    char map[MAX_ROWS][MAX_COLS];
    int rows, cols;

    readMapFromFile("map.txt", map, &rows, &cols);
    printMap(map, rows, cols);

    return 0;
}