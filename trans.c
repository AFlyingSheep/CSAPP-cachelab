/*
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */
#include "cachelab.h"
#include <stdio.h>

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

/*
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded.
 */
char transpose_submit_desc[] = "Transpose submission";
#define ROW_BLOCK_SIZE_32 8
#define COL_BLOCK_SIZE_32 8
#define ROW_BLOCK_SIZE_64 8
#define COL_BLOCK_SIZE_64 8
#define ROW_BLOCK_SIZE_UN 16
#define COL_BLOCK_SIZE_UN 2

#define MIN(a, b) ((a < b) ? a : b)

void transpose_submit(int M, int N, int A[N][M], int B[M][N]) {
    if (M == 32 && N == 32) {
        int tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7;
        for (int i = 0; i < N; i += ROW_BLOCK_SIZE_32) {
            for (int j = 0; j < M; j += COL_BLOCK_SIZE_32) {
                if (i != j) {
                    for (int ii = i; ii < MIN((i + ROW_BLOCK_SIZE_32), N);
                         ii++) {
                        for (int jj = j; jj < MIN((j + COL_BLOCK_SIZE_32), M);
                             jj++) {
                            tmp0      = A[jj][ii];
                            B[ii][jj] = tmp0;
                        }
                    }
                } else {
                    for (int ii = i; ii < MIN((i + ROW_BLOCK_SIZE_32), N);
                         ii++) {
                        tmp0 = B[j][ii];
                        tmp1 = B[j + 1][ii];
                        tmp2 = B[j + 2][ii];
                        tmp3 = B[j + 3][ii];
                        tmp4 = B[j + 4][ii];
                        tmp5 = B[j + 5][ii];
                        tmp6 = B[j + 6][ii];
                        tmp7 = B[j + 7][ii];

                        A[ii][j]     = tmp0;
                        A[ii][j + 1] = tmp1;
                        A[ii][j + 2] = tmp2;
                        A[ii][j + 3] = tmp3;
                        A[ii][j + 4] = tmp4;
                        A[ii][j + 5] = tmp5;
                        A[ii][j + 6] = tmp6;
                        A[ii][j + 7] = tmp7;
                    }
                }
            }
        }
    } else if (M == 64 && N == 64) {
        for (int i = 0; i < N; i += 8) {
            for (int j = 0; j < M; j += 8) {
                int tmp;
                int tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7;

                // up-left
                {
                    tmp0 = B[i][j];
                    tmp1 = B[i][j + 1];
                    tmp2 = B[i][j + 2];
                    tmp3 = B[i][j + 3];

                    tmp4 = B[i + 1][j];
                    tmp5 = B[i + 1][j + 1];
                    tmp6 = B[i + 1][j + 2];
                    tmp7 = B[i + 1][j + 3];

                    A[j][i]     = tmp0;
                    A[j + 1][i] = tmp1;
                    A[j + 2][i] = tmp2;
                    A[j + 3][i] = tmp3;

                    A[j][i + 1]     = tmp4;
                    A[j + 1][i + 1] = tmp5;
                    A[j + 2][i + 1] = tmp6;
                    A[j + 3][i + 1] = tmp7;

                    tmp0 = B[i + 2][j];
                    tmp1 = B[i + 2][j + 1];
                    tmp2 = B[i + 2][j + 2];
                    tmp3 = B[i + 2][j + 3];

                    tmp4 = B[i + 3][j];
                    tmp5 = B[i + 3][j + 1];
                    tmp6 = B[i + 3][j + 2];
                    tmp7 = B[i + 3][j + 3];

                    A[j][i + 2]     = tmp0;
                    A[j + 1][i + 2] = tmp1;
                    A[j + 2][i + 2] = tmp2;
                    A[j + 3][i + 2] = tmp3;

                    A[j][i + 3]     = tmp4;
                    A[j + 1][i + 3] = tmp5;
                    A[j + 2][i + 3] = tmp6;
                    A[j + 3][i + 3] = tmp7;
                }

                // up-right but not right position
                // for (int ii = i; ii < i + 4; ii++) {
                //     for (int jj = j + 4; jj < j + 8; jj++) {
                //         tmp               = B[ii][jj];
                //         A[jj - 4][ii + 4] = tmp;
                //     }
                // }
                {
                    tmp0 = B[i][j + 4];
                    tmp1 = B[i][j + 5];
                    tmp2 = B[i][j + 6];
                    tmp3 = B[i][j + 7];

                    tmp4 = B[i + 1][j + 4];
                    tmp5 = B[i + 1][j + 5];
                    tmp6 = B[i + 1][j + 6];
                    tmp7 = B[i + 1][j + 7];

                    A[j][i + 4]     = tmp0;
                    A[j + 1][i + 4] = tmp1;
                    A[j + 2][i + 4] = tmp2;
                    A[j + 3][i + 4] = tmp3;

                    A[j][i + 5]     = tmp4;
                    A[j + 1][i + 5] = tmp5;
                    A[j + 2][i + 5] = tmp6;
                    A[j + 3][i + 5] = tmp7;

                    tmp0 = B[i + 2][j + 4];
                    tmp1 = B[i + 2][j + 5];
                    tmp2 = B[i + 2][j + 6];
                    tmp3 = B[i + 2][j + 7];

                    tmp4 = B[i + 3][j + 4];
                    tmp5 = B[i + 3][j + 5];
                    tmp6 = B[i + 3][j + 6];
                    tmp7 = B[i + 3][j + 7];

                    A[j][i + 6]     = tmp0;
                    A[j + 1][i + 6] = tmp1;
                    A[j + 2][i + 6] = tmp2;
                    A[j + 3][i + 6] = tmp3;

                    A[j][i + 7]     = tmp4;
                    A[j + 1][i + 7] = tmp5;
                    A[j + 2][i + 7] = tmp6;
                    A[j + 3][i + 7] = tmp7;
                }

                // move up-right to down-left by tmps
                {
                    tmp0 = A[j][i + 4];
                    tmp1 = A[j][i + 5];
                    tmp2 = A[j][i + 6];
                    tmp3 = A[j][i + 7];

                    tmp4 = A[j + 1][i + 4];
                    tmp5 = A[j + 1][i + 5];
                    tmp6 = A[j + 1][i + 6];
                    tmp7 = A[j + 1][i + 7];

                    for (int jj = j; jj < j + 2; jj++) {
                        for (int ii = i + 4; ii < i + 8; ii++) {
                            tmp       = B[ii][jj];
                            A[jj][ii] = tmp;
                        }
                    }

                    A[j + 4][i]     = tmp0;
                    A[j + 4][i + 1] = tmp1;
                    A[j + 4][i + 2] = tmp2;
                    A[j + 4][i + 3] = tmp3;

                    A[j + 5][i]     = tmp4;
                    A[j + 5][i + 1] = tmp5;
                    A[j + 5][i + 2] = tmp6;
                    A[j + 5][i + 3] = tmp7;

                    tmp0 = A[j + 2][i + 4];
                    tmp1 = A[j + 2][i + 5];
                    tmp2 = A[j + 2][i + 6];
                    tmp3 = A[j + 2][i + 7];

                    tmp4 = A[j + 3][i + 4];
                    tmp5 = A[j + 3][i + 5];
                    tmp6 = A[j + 3][i + 6];
                    tmp7 = A[j + 3][i + 7];

                    for (int jj = j + 2; jj < j + 4; jj++) {
                        for (int ii = i + 4; ii < i + 8; ii++) {
                            tmp       = B[ii][jj];
                            A[jj][ii] = tmp;
                        }
                    }

                    A[j + 6][i]     = tmp0;
                    A[j + 6][i + 1] = tmp1;
                    A[j + 6][i + 2] = tmp2;
                    A[j + 6][i + 3] = tmp3;

                    A[j + 7][i]     = tmp4;
                    A[j + 7][i + 1] = tmp5;
                    A[j + 7][i + 2] = tmp6;
                    A[j + 7][i + 3] = tmp7;
                }

                // move down-right
                {
                    tmp0 = B[i + 4][j + 4];
                    tmp1 = B[i + 4][j + 5];
                    tmp2 = B[i + 4][j + 6];
                    tmp3 = B[i + 4][j + 7];

                    tmp4 = B[i + 5][j + 4];
                    tmp5 = B[i + 5][j + 5];
                    tmp6 = B[i + 5][j + 6];
                    tmp7 = B[i + 5][j + 7];

                    A[j + 4][i + 4] = tmp0;
                    A[j + 5][i + 4] = tmp1;
                    A[j + 6][i + 4] = tmp2;
                    A[j + 7][i + 4] = tmp3;

                    A[j + 4][i + 5] = tmp4;
                    A[j + 5][i + 5] = tmp5;
                    A[j + 6][i + 5] = tmp6;
                    A[j + 7][i + 5] = tmp7;

                    tmp0 = B[i + 6][j + 4];
                    tmp1 = B[i + 6][j + 5];
                    tmp2 = B[i + 6][j + 6];
                    tmp3 = B[i + 6][j + 7];

                    tmp4 = B[i + 7][j + 4];
                    tmp5 = B[i + 7][j + 5];
                    tmp6 = B[i + 7][j + 6];
                    tmp7 = B[i + 7][j + 7];

                    A[j + 4][i + 6] = tmp0;
                    A[j + 5][i + 6] = tmp1;
                    A[j + 6][i + 6] = tmp2;
                    A[j + 7][i + 6] = tmp3;

                    A[j + 4][i + 7] = tmp4;
                    A[j + 5][i + 7] = tmp5;
                    A[j + 6][i + 7] = tmp6;
                    A[j + 7][i + 7] = tmp7;
                }
            }
            // }
        }
    } else {
        int tmp0;
        for (int i = 0; i < N; i += ROW_BLOCK_SIZE_UN) {
            for (int j = 0; j < M; j += COL_BLOCK_SIZE_UN) {
                for (int ii = i; ii < MIN((i + ROW_BLOCK_SIZE_UN), N); ii++) {
                    for (int jj = j; jj < MIN((j + COL_BLOCK_SIZE_UN), M);
                         jj++) {
                        tmp0      = B[jj][ii];
                        A[ii][jj] = tmp0;
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
 * trans - A simple baseline transpose function, not optimized for the
 * cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N]) {
    int i, j, tmp;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            tmp     = A[i][j];
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
    // registerTransFunction(trans, trans_desc);
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
