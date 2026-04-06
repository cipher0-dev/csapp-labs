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
#include <stdlib.h>

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

#define printf_debug(...)
// #define printf_debug(...) printf(__VA_ARGS__)

void print_matrix(int M, int N, int A[M][N]) {
  printf_debug("rows: %d, cols: %d:\n", M, N);
  for (auto i = 0; i < M; i++) {
    for (auto j = 0; j < N; j++) {
      printf_debug(" %3d", A[i][j]);
    }
    printf_debug("\n");
  }
  printf_debug("\n");
}

int min(int a, int b) { return (a < b) ? a : b; }
int max(int a, int b) { return (a < b) ? b : a; }

/*
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded.
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N]) {
  // initialize input array to sequential values and output array to all -1 for
  // easier debugging
  // auto num = 0;
  // for (auto i = 0; i < N; i++) {
  //   for (auto j = 0; j < M; j++) {
  //     A[i][j] = ++num;
  //     printf_debug("A[%d][%d]: %d\n", i, j, A[i][j]);
  //   }
  // }
  // for (auto i = 0; i < M; i++) {
  //   for (auto j = 0; j < N; j++) {
  //     B[i][j] = -1;
  //   }
  // }

  // const int batch_components = 4;
  // const int min_batch_size = 2;
  //
  // auto a_batch_rows = min(max(N / batch_components, min_batch_size), N);
  // auto a_batch_cols = min(max(M / batch_components, min_batch_size), M);
  auto a_batch_rows = 16;
  auto a_batch_cols = 4;
  if (M == 32 && N == 32) {
    a_batch_rows = 8;
    a_batch_cols = 8;
  } else if (M == 64 && N == 64) {
    a_batch_rows = 8;
    a_batch_cols = 4;
  }

  printf_debug("batch size: [%d][%d]\n", a_batch_rows, a_batch_cols);

  printf_debug("remaining rows: %d\n", N % a_batch_rows);
  printf_debug("remaining cols: %d\n", M % a_batch_cols);

  for (auto i = 0; i < N / a_batch_rows; i++) {
    for (auto j = 0; j < M / a_batch_cols; j++) {
      printf_debug("batch: [%d][%d]\n", i, j);
      for (auto k = 0; k < a_batch_rows; k++) {
        auto a_row = i * a_batch_rows + k;
        auto b_col = a_row;
        for (auto l = 0; l < a_batch_cols; l++) {
          auto a_col = j * a_batch_cols + l;
          auto b_row = a_col;

          printf_debug("moving: A[%d][%d] to B[%d][%d]: %d\n", a_row, a_col,
                       b_row, b_col, A[a_row][a_col]);
          B[b_row][b_col] = A[a_row][a_col];
        }
      }
    }

    // transpose remaining batch for this row
    printf_debug("remaining batch: [%d][%d]\n", i, M / a_batch_cols);
    for (auto k = 0; k < a_batch_rows; k++) {
      auto a_row = i * a_batch_rows + k;
      auto b_col = a_row;

      auto remaining_col_pos = M / a_batch_cols * a_batch_cols;
      for (auto l = remaining_col_pos; l < M; l++) {
        auto a_col = l;
        auto b_row = a_col;

        printf_debug("moving: A[%d][%d] to B[%d][%d]: %d\n", a_row, a_col,
                     b_row, b_col, A[a_row][a_col]);
        B[b_row][b_col] = A[a_row][a_col];
      }
    }
  }
  // transpose remaining rows
  // TODO: batch reamining rows to have more cache hits
  printf_debug("remaining rows: [%d][%d]\n", N / a_batch_rows, 0);
  auto remaining_row_pos = N / a_batch_rows * a_batch_rows;
  for (auto i = remaining_row_pos; i < N; i++) {
    for (auto j = 0; j < M; j++) {
      auto a_row = i;
      auto b_col = a_row;
      auto a_col = j;
      auto b_row = a_col;

      printf_debug("moving: A[%d][%d] to B[%d][%d]: %d\n", a_row, a_col, b_row,
                   b_col, A[a_row][a_col]);
      B[b_row][b_col] = A[a_row][a_col];
    }
  }

  print_matrix(N, M, A);
  print_matrix(M, N, B);
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
