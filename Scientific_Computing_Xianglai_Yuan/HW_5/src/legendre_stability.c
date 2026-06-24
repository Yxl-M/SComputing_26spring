/*
 * HomeWork 05 - Stability of Legendre Polynomials
 * Scientific Computing
 *
 * This program compares three numerical sequences for ordinary Legendre
 * polynomials P_l(x), for l = 0,...,lmax and selected x values:
 *   1. Forward Bonnet recurrence in double precision.
 *   2. A higher-precision reference using long double recurrence.
 *   3. A Miller-style backward recurrence experiment followed by scaling Q[0] = 1.
 *
 * 本程序比较普通 Legendre 多项式 P_l(x) 的三种数值计算方式：
 *   1. double 精度下的正向 Bonnet 三项递推；
 *   2. 使用 long double 的较高精度参考值；
 *   3. Miller 风格的反向递推实验，并通过 Q[0] = 1 归一化。
 */

#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef M_PI
#define M_PI 3.141592653589793238462643383279502884L
#endif

/* Compute P_l(x) by forward Bonnet recurrence in double precision.
 * 使用 double 精度的 Bonnet 正向递推计算 P_l(x)。 */
static void legendre_forward_double(double x, int lmax, double *P) {
    P[0] = 1.0;
    if (lmax == 0) {
        return;
    }

    P[1] = x;
    for (int l = 1; l < lmax; ++l) {
        P[l + 1] = ((2.0 * l + 1.0) * x * P[l] - (double)l * P[l - 1]) /
                   (double)(l + 1);
    }
}

/* Compute reference values with long double arithmetic.
 * 用 long double 算术计算参考值，比 double 更精确，但不是任意精度。
 * For a true 100-decimal-digit reference, see scripts/legendre_mpmath_analysis.py. */
static void legendre_reference_long_double(double x, int lmax, long double *Pref) {
    const long double xl = (long double)x;

    Pref[0] = 1.0L;
    if (lmax == 0) {
        return;
    }

    Pref[1] = xl;
    for (int l = 1; l < lmax; ++l) {
        Pref[l + 1] = (((long double)(2 * l + 1)) * xl * Pref[l]
                       - (long double)l * Pref[l - 1]) /
                      (long double)(l + 1);
    }
}

/* Miller-style backward recurrence experiment.
 * Miller 风格反向递推实验。
 * Important / 重要：
 * This is not expected to recover ordinary P_l(x) reliably for |x| < 1.
 * 对于 |x| < 1 的普通 Legendre 多项式，这个任意初值的反向递推通常不能可靠恢复 P_l(x)。 */
static void legendre_backward_experiment(double x, int lmax, int L, double *Pback) {
    if (L <= lmax) {
        fprintf(stderr, "Error: backward starting degree L must be larger than lmax.\n");
        exit(EXIT_FAILURE);
    }

    double *Q = calloc((size_t)(L + 2), sizeof(*Q));
    if (Q == NULL) {
        fprintf(stderr, "Error: memory allocation failed in backward experiment.\n");
        exit(EXIT_FAILURE);
    }

    /* Arbitrary terminal conditions / 任意的末端初值 */
    Q[L + 1] = 0.0;
    Q[L] = 1.0;

    /* Backward form of Bonnet recurrence:
     * P_{l-1} = ((2l+1) x P_l - (l+1) P_{l+1}) / l
     * Bonnet 递推式的反向形式。 */
    for (int l = L; l >= 1; --l) {
        Q[l - 1] = ((2.0 * l + 1.0) * x * Q[l] - (double)(l + 1) * Q[l + 1]) /
                   (double)l;
    }

    if (fabs(Q[0]) < DBL_MIN) {
        fprintf(stderr, "Error: Q[0] is too close to zero; cannot rescale safely.\n");
        free(Q);
        exit(EXIT_FAILURE);
    }

    /* Rescale so that P_0 = 1 / 重新缩放，使得 P_0 = 1。 */
    const double scale = 1.0 / Q[0];
    for (int l = 0; l <= lmax; ++l) {
        Pback[l] = scale * Q[l];
    }

    free(Q);
}

/* Relative error with protection near zero.
 * 相对误差；当参考值非常接近 0 时，改用绝对误差避免除以极小数。 */
static long double relative_error_ld(long double value, long double reference) {
    const long double abs_err = fabsl(value - reference);
    if (fabsl(reference) > LDBL_MIN) {
        return abs_err / fabsl(reference);
    }
    return abs_err;
}

/* Write one CSV row.
 * 写出一行 CSV 数据。 */
static void write_csv_row(FILE *fp, double x, int l,
                          long double ref, double forward, double backward) {
    const long double fwd = (long double)forward;
    const long double bwd = (long double)backward;

    const long double abs_err_fwd = fabsl(fwd - ref);
    const long double abs_err_bwd = fabsl(bwd - ref);
    const long double rel_err_fwd = relative_error_ld(fwd, ref);
    const long double rel_err_bwd = relative_error_ld(bwd, ref);

    /* %Le prints long double in scientific notation.
     * %Le 用科学计数法打印 long double。 */
    fprintf(fp,
            "%.17g,%d,%.21Le,%.17e,%.17e,%.21Le,%.21Le,%.21Le,%.21Le\n",
            x, l, ref, forward, backward,
            abs_err_fwd, rel_err_fwd, abs_err_bwd, rel_err_bwd);
}

int main(int argc, char **argv) {
    int lmax = 50;
    int L = 80;
    const char *output_file = "results/legendre_errors_c.csv";

    if (argc >= 2) {
        lmax = atoi(argv[1]);
    }
    if (argc >= 3) {
        L = atoi(argv[2]);
    }
    if (argc >= 4) {
        output_file = argv[3];
    }

    if (lmax < 1) {
        fprintf(stderr, "Error: lmax must be at least 1.\n");
        return EXIT_FAILURE;
    }
    if (L <= lmax) {
        fprintf(stderr, "Error: L must be larger than lmax.\n");
        return EXIT_FAILURE;
    }

    const double xs[] = {0.1, 0.5, 0.9, 0.99};
    const int nx = (int)(sizeof(xs) / sizeof(xs[0]));

    double *Pforward = malloc((size_t)(lmax + 1) * sizeof(*Pforward));
    double *Pback = malloc((size_t)(lmax + 1) * sizeof(*Pback));
    long double *Pref = malloc((size_t)(lmax + 1) * sizeof(*Pref));

    if (Pforward == NULL || Pback == NULL || Pref == NULL) {
        fprintf(stderr, "Error: memory allocation failed.\n");
        free(Pforward);
        free(Pback);
        free(Pref);
        return EXIT_FAILURE;
    }

    FILE *fp = fopen(output_file, "w");
    if (fp == NULL) {
        fprintf(stderr, "Error: cannot open output file '%s'.\n", output_file);
        free(Pforward);
        free(Pback);
        free(Pref);
        return EXIT_FAILURE;
    }

    fprintf(fp, "x,l,P_reference,P_forward,P_backward,abs_err_forward,rel_err_forward,abs_err_backward,rel_err_backward\n");

    for (int ix = 0; ix < nx; ++ix) {
        const double x = xs[ix];

        legendre_reference_long_double(x, lmax, Pref);
        legendre_forward_double(x, lmax, Pforward);
        legendre_backward_experiment(x, lmax, L, Pback);

        for (int l = 0; l <= lmax; ++l) {
            write_csv_row(fp, x, l, Pref[l], Pforward[l], Pback[l]);
        }
    }

    fclose(fp);
    free(Pforward);
    free(Pback);
    free(Pref);

    printf("Wrote C results to %s\n", output_file);
    printf("lmax = %d, backward starting L = %d\n", lmax, L);
    printf("Conclusion to check in the CSV: forward recurrence stays close to the reference; backward experiment is generally inaccurate.\n");

    return EXIT_SUCCESS;
}
