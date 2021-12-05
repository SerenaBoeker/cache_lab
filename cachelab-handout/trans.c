/*
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */
#include <stdio.h>
#include "cachelab.h"

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

/*
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded.
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N]) {
    int diag_value = 0;
    int diag_value_pos = 0;
    int matrix_col;
    int matrix_row;
    int block_row;
    int block_col;
    if(N == 32) {

        for(matrix_col = 0; matrix_col < 32; matrix_col += 8) {
            for(matrix_row = 0; matrix_row < 32; matrix_row += 8) {

                for(block_row = matrix_row; block_row < matrix_row + 8; block_row++) {
                    for(block_col = matrix_col; block_col < matrix_col + 8; block_col++) {

                        if(block_row == block_col) {
                            diag_value = A[block_row][block_col];
                            diag_value_pos = block_row;
                        } else {
                            B[block_col][block_row] = A[block_row][block_col];
                        }
                    }
                    if(matrix_col == matrix_row) {
                        B[diag_value_pos][diag_value_pos] = diag_value;
                    }
                }
            }
        }
    } else if (N == 64) {

        for(matrix_col = 0; matrix_col < 64; matrix_col += 4) {
            for(matrix_row = 0; matrix_row < 64; matrix_row += 4) {

                for(block_row = matrix_row; block_row < matrix_row + 4; block_row++) {
                    for(block_col = matrix_col; block_col < matrix_col + 4; block_col++) {

                        if(block_row == block_col) {
                            diag_value = A[block_row][block_col];
                            diag_value_pos = block_row;
                        } else {
                            B[block_col][block_row] = A[block_row][block_col];
                        }
                    }
                    if(matrix_col == matrix_row) {
                        B[diag_value_pos][diag_value_pos] = diag_value;
                    }
                }
            }
        }
    } else if (N == 67 && M == 61) {
        for(matrix_col = 0; matrix_col < N; matrix_col += 16) {
            for(matrix_row = 0; matrix_row < N; matrix_row += 16) {

                for(block_row = matrix_row; (block_row < matrix_row + 16) && (block_row < N); block_row++) {
                    for(block_col = matrix_col; (block_col < matrix_col + 16) && (block_col < M); block_col++) {

                        if(block_row == block_col) {
                            diag_value = A[block_row][block_col];
                            diag_value_pos = block_row;
                        } else {
                            B[block_col][block_row] = A[block_row][block_col];
                        }
                    }
                    if(matrix_col == matrix_row) {
                        B[diag_value_pos][diag_value_pos] = diag_value;
                    }
                }
            }
        }
    }

}

/*
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started.
 */

/*
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N]) {
    int i, j, tmp;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }
}

/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions() {
    /* Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc);

    /* Register any additional transpose functions */
    registerTransFunction(trans, trans_desc);
}

/*
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N]) {
    int i, j;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; ++j) {
            if (A[i][j] != B[j][i]) {
                return 0;
            }
        }
    }
    return 1;
}
