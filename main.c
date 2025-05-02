#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define SIZE 9

int sudoku[SIZE][SIZE];

// Thread result flags
int row_valid[SIZE] = {0};
int col_valid[SIZE] = {0};
int box_valid[SIZE] = {0};

// Struct to pass parameters to threads
typedef struct {
    int index;
    int row;
    int col;
} parameters;

void* check_row(void* arg) {
    parameters* p = (parameters*) arg;
    int seen[SIZE + 1] = {0};

    for (int j = 0; j < SIZE; j++) {
        int num = sudoku[p->row][j];
        if (num < 1 || num > 9 || seen[num])
            pthread_exit(NULL);
        seen[num] = 1;
    }
    row_valid[p->index] = 1;
    pthread_exit(NULL);
}

void* check_col(void* arg) {
    parameters* p = (parameters*) arg;
    int seen[SIZE + 1] = {0};

    for (int i = 0; i < SIZE; i++) {
        int num = sudoku[i][p->col];
        if (num < 1 || num > 9 || seen[num])
            pthread_exit(NULL);
        seen[num] = 1;
    }
    col_valid[p->index] = 1;
    pthread_exit(NULL);
}

void* check_box(void* arg) {
    parameters* p = (parameters*) arg;
    int seen[SIZE + 1] = {0};

    for (int i = p->row; i < p->row + 3; i++) {
        for (int j = p->col; j < p->col + 3; j++) {
            int num = sudoku[i][j];
            if (num < 1 || num > 9 || seen[num])
                pthread_exit(NULL);
            seen[num] = 1;
        }
    }
    box_valid[p->index] = 1;
    pthread_exit(NULL);
}

int main() {
    printf("Enter the Sudoku (9x9), row by row:\n");
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            scanf("%d", &sudoku[i][j]);
        }
    }
    pthread_t threads[27];
    parameters* data[27];
    int thread_idx = 0;
    // Create 9 row-checking threads
    for (int i = 0; i < SIZE; i++) {
        data[thread_idx] = malloc(sizeof(parameters));
        data[thread_idx]->index = i;
        data[thread_idx]->row = i;
        pthread_create(&threads[thread_idx], NULL, check_row, data[thread_idx]);
        thread_idx++;
    }
    // Create 9 column-checking threads
    for (int i = 0; i < SIZE; i++) {
        data[thread_idx] = malloc(sizeof(parameters));
        data[thread_idx]->index = i;
        data[thread_idx]->col = i;
        pthread_create(&threads[thread_idx], NULL, check_col, data[thread_idx]);
        thread_idx++;
    }
    // Create 9 box-checking threads
    int box_idx = 0;
    for (int i = 0; i < SIZE; i += 3) {
        for (int j = 0; j < SIZE; j += 3) {
            data[thread_idx] = malloc(sizeof(parameters));
            data[thread_idx]->index = box_idx;
            data[thread_idx]->row = i;
            data[thread_idx]->col = j;
            pthread_create(&threads[thread_idx], NULL, check_box, data[thread_idx]);
            thread_idx++;
            box_idx++;
        }
    }
    // Wait for threads to finish
    for (int i = 0; i < 27; i++) {
        pthread_join(threads[i], NULL);
        free(data[i]);
    }
    int valid = 1;
    for (int i = 0; i < SIZE; i++) {
        if (!row_valid[i] || !col_valid[i] || !box_valid[i]) {
            valid = 0;
            break;
        }
    }
    if (valid) {
        printf("The Sudoku is valid.");
    }
    else {
        printf("The Sudoku is invalid.");
    }
    return 0;
}
