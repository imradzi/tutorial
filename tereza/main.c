#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

static int N;
#define SEPARATOR "------------------------------------\n"
#define init_path "init_data.txt"
#define A_path "A.txt"

// Return the current time in seconds since the Epoch
double get_timestamp();

void get_2D_array(float **A, int N1, int N2, char *name);

void run(float **A) {
  float c;
  float sum;
  float *x = malloc(N * sizeof(float));

  // The generation of upper triangular matrix

  for (int j = 0; j < N + 1; j++) {
    for (int i = j + 1; i < N + 1; i++) {
      c = A[i][j] / A[j][j];
      for (int k = 0; k < N; k++) {
        A[i][k] = A[i][k] - c * A[j][k];
      }
    }
  }
  x[N] = A[N][N + 1] / A[N][N];

  // Backward substitution

  for (int i = N - 1; i > 0; i--) {
    sum = 0;
    for (int j = i + 1; j < N + 1; j++) {
      sum = sum + A[i][j] * x[j];
    }
    x[i] = (A[i][N + 1] - sum) / A[i][i];
  }
  printf("\nThe solution is: \n");
  for (int i = 1; i < N + 1; i++) {
    printf("\nx%d=%f\t", i, x[i]); /* x1, x2, x3 are the required solutions*/
  }
}

// Funce na zadání počátečních dat - velikost matice
void get_init_data(char *name) {
  FILE *in = fopen(name, "r");
  if ((in) == NULL) {
    printf("Cannot open file %s.\n", name);
  }
  fscanf(in, "%d", &N);
  fclose(in);
}

void get_2D_array(float **A, int N1, int N2, char *name) {
  int i, j;
  FILE *in = fopen(name, "r");

  if ((in) == NULL) {
    printf("Cannot open file %s.\n", name);
  }
  for (i = 0; i < N1; i++) {
    for (j = 0; j < N2; j++) {
      fscanf(in, "%f", &A[i][j]);
    }
  }
  fclose(in);
}

// Program

int main(int argc, char *argv[]) {
  int i, j;
  get_init_data(init_path);
  float **A = (float **)malloc(N * sizeof(float *));
  for (i = 0; i < N; i++) {
    A[i] = (float *)malloc(N * sizeof(float));
  }

  printf(SEPARATOR);
  printf("Matrix size:            %dx%d\n", N, N);
  printf(SEPARATOR);

  double total_start = get_timestamp();

  // Initialize data
  get_2D_array(A, N, N, A_path);

  // Run Gaussian elimination with measured time
  double solve_start = get_timestamp();
  run(A);
  double solve_end = get_timestamp();

  for (int i = 0; i < N; i++) {
    free(A[i]);
  }
  free(A);
  return 0;
}

double get_timestamp() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return tv.tv_sec + tv.tv_usec * 1e-6;
}
