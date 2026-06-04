#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

static double now_seconds(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec * 1e-9;
}

static double *alloc_matrix(long long N)
{
    double *M = (double *)malloc((size_t)N * (size_t)N * sizeof(double));
    if (M == NULL)
    {
        printf("Memory allocation failed for N = %lld\n", N);
        exit(1);
    }
    return M;
}

static void fill_matrix(double *M, long long N, double val)
{
    for (long long i = 0; i < N * N; i++)
        M[i] = val;
}

static void zero_matrix(double *M, long long N)
{
    for (long long i = 0; i < N * N; i++)
        M[i] = 0.0;
}

static void save_matrix(const char *filename, double *C, long long N)
{
    FILE *fp = fopen(filename, "w");
    if (fp == NULL)
    {
        printf("Cannot open output file: %s\n", filename);
        exit(1);
    }

    for (long long i = 0; i < N; i++)
    {
        for (long long j = 0; j < N; j++)
        {
            fprintf(fp, "%.6f", C[i * N + j]);
            if (j < N - 1) fprintf(fp, " ");
        }
        fprintf(fp, "\n");
    }

    fclose(fp);
}

static int check_matrix(double *C, long long N, double expected, double tol)
{
    double minv = C[0];
    double maxv = C[0];

    for (long long i = 0; i < N * N; i++)
    {
        if (C[i] < minv) minv = C[i];
        if (C[i] > maxv) maxv = C[i];
    }

    printf("Check: min(C) = %.17g, max(C) = %.17g, expected = %.17g\n",
           minv, maxv, expected);

    if (fabs(minv - expected) <= tol && fabs(maxv - expected) <= tol)
        return 1;
    return 0;
}

/* ijk */
static void matmul_ijk(double *A, double *B, double *C, long long N)
{
    zero_matrix(C, N);
    for (long long i = 0; i < N; i++)
        for (long long j = 0; j < N; j++)
            for (long long k = 0; k < N; k++)
                C[i * N + j] += A[i * N + k] * B[k * N + j];
}

/* ikj */
static void matmul_ikj(double *A, double *B, double *C, long long N)
{
    zero_matrix(C, N);
    for (long long i = 0; i < N; i++)
        for (long long k = 0; k < N; k++)
        {
            double aik = A[i * N + k];
            for (long long j = 0; j < N; j++)
                C[i * N + j] += aik * B[k * N + j];
        }
}

/* jik */
static void matmul_jik(double *A, double *B, double *C, long long N)
{
    zero_matrix(C, N);
    for (long long j = 0; j < N; j++)
        for (long long i = 0; i < N; i++)
            for (long long k = 0; k < N; k++)
                C[i * N + j] += A[i * N + k] * B[k * N + j];
}

/* jki */
static void matmul_jki(double *A, double *B, double *C, long long N)
{
    zero_matrix(C, N);
    for (long long j = 0; j < N; j++)
        for (long long k = 0; k < N; k++)
        {
            double bkj = B[k * N + j];
            for (long long i = 0; i < N; i++)
                C[i * N + j] += A[i * N + k] * bkj;
        }
}

/* kij */
static void matmul_kij(double *A, double *B, double *C, long long N)
{
    zero_matrix(C, N);
    for (long long k = 0; k < N; k++)
        for (long long i = 0; i < N; i++)
        {
            double aik = A[i * N + k];
            for (long long j = 0; j < N; j++)
                C[i * N + j] += aik * B[k * N + j];
        }
}

/* kji */
static void matmul_kji(double *A, double *B, double *C, long long N)
{
    zero_matrix(C, N);
    for (long long k = 0; k < N; k++)
        for (long long j = 0; j < N; j++)
        {
            double bkj = B[k * N + j];
            for (long long i = 0; i < N; i++)
                C[i * N + j] += A[i * N + k] * bkj;
        }
}

int main(int argc, char *argv[])
{
    if (argc != 5)
    {
        printf("Usage: %s a b N fileout\n", argv[0]);
        printf("Example: %s 2.0 3.0 4 out.txt\n", argv[0]);
        return 1;
    }

    double a = atof(argv[1]);
    double b = atof(argv[2]);
    long long N = atoll(argv[3]);
    const char *fileout = argv[4];

    if (N <= 0)
    {
        printf("Error: N must be positive.\n");
        return 1;
    }

    printf("Input: a = %.17g, b = %.17g, N = %lld, fileout = %s\n",
           a, b, N, fileout);

    double *A = alloc_matrix(N);
    double *B = alloc_matrix(N);
    double *C = alloc_matrix(N);

    fill_matrix(A, N, a);
    fill_matrix(B, N, b);

    double expected = (double)N * a * b;
    double tol = 1e-9;

    double t1, t2;

    t1 = now_seconds();
    matmul_ijk(A, B, C, N);
    t2 = now_seconds();
    printf("ijk : %.6f s\n", t2 - t1);
    printf("ijk correct? %s\n", check_matrix(C, N, expected, tol) ? "YES" : "NO");

    t1 = now_seconds();
    matmul_ikj(A, B, C, N);
    t2 = now_seconds();
    printf("ikj : %.6f s\n", t2 - t1);
    printf("ikj correct? %s\n", check_matrix(C, N, expected, tol) ? "YES" : "NO");

    t1 = now_seconds();
    matmul_jik(A, B, C, N);
    t2 = now_seconds();
    printf("jik : %.6f s\n", t2 - t1);
    printf("jik correct? %s\n", check_matrix(C, N, expected, tol) ? "YES" : "NO");

    t1 = now_seconds();
    matmul_jki(A, B, C, N);
    t2 = now_seconds();
    printf("jki : %.6f s\n", t2 - t1);
    printf("jki correct? %s\n", check_matrix(C, N, expected, tol) ? "YES" : "NO");

    t1 = now_seconds();
    matmul_kij(A, B, C, N);
    t2 = now_seconds();
    printf("kij : %.6f s\n", t2 - t1);
    printf("kij correct? %s\n", check_matrix(C, N, expected, tol) ? "YES" : "NO");

    t1 = now_seconds();
    matmul_kji(A, B, C, N);
    t2 = now_seconds();
    printf("kji : %.6f s\n", t2 - t1);
    printf("kji correct? %s\n", check_matrix(C, N, expected, tol) ? "YES" : "NO");

    save_matrix(fileout, C, N);
    printf("Matrix C saved to %s\n", fileout);

    free(A);
    free(B);
    free(C);

    return 0;
}
