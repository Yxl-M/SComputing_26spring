/*
 * HomeWork 02 - Dynamic-memory version
 * 作业 02 - 动态内存版本
 *
 * Computes the vector operation: d = a*x + y
 * 计算向量运算：d = a*x + y
 *
 * Command line / 命令行格式:
 *     ./sum2vec_dynamic.x N a x y
 * Example / 示例:
 *     ./sum2vec_dynamic.x 1000000 3 0.1 7.1
 *
 * This version uses malloc/free, so the vectors are allocated on the heap.
 * 本版本使用 malloc/free，因此向量分配在堆内存上。
 * It is more suitable for large vectors than the stack-memory version.
 * 与栈内存版本相比，它更适合较大的向量。
 */

#include <errno.h>
#include <limits.h>
#include <stdint.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define TOLERANCE 1e-12

/* Parse a positive integer N from text.
 * 从字符串中解析正整数 N。 */
long parse_positive_long(const char *text)
{
    char *endptr = NULL;
    errno = 0;
    long value = strtol(text, &endptr, 10);

    /* Check whether the whole string was converted correctly.
     * 检查整个字符串是否被正确转换。 */
    if (errno != 0 || endptr == text || *endptr != '\0' || value <= 0) {
        fprintf(stderr, "Error: N must be a positive integer.\n");
        exit(EXIT_FAILURE);
    }

    return value;
}

/* Parse a double-precision number from text.
 * 从字符串中解析 double 类型浮点数。 */
double parse_double(const char *text, const char *name)
{
    char *endptr = NULL;
    errno = 0;
    double value = strtod(text, &endptr);

    /* If conversion fails, print which argument is invalid.
     * 如果转换失败，打印是哪一个参数无效。 */
    if (errno != 0 || endptr == text || *endptr != '\0') {
        fprintf(stderr, "Error: %s must be a valid number.\n", name);
        exit(EXIT_FAILURE);
    }

    return value;
}

/* Allocate one vector and check possible memory errors.
 * 分配一个向量，并检查可能的内存错误。 */
double *allocate_vector(long N, const char *name)
{
    /* Avoid integer overflow before multiplying N by sizeof(double).
     * 在计算 N * sizeof(double) 之前避免整数溢出。 */
    if ((unsigned long)N > (unsigned long)SIZE_MAX / sizeof(double)) {
        fprintf(stderr, "Error: vector %s is too large.\n", name);
        exit(EXIT_FAILURE);
    }

    double *v = malloc((size_t)N * sizeof(double));

    /* malloc returns NULL when allocation fails.
     * 当内存分配失败时，malloc 会返回 NULL。 */
    if (v == NULL) {
        fprintf(stderr, "Error: memory allocation failed for vector %s.\n", name);
        exit(EXIT_FAILURE);
    }

    return v;
}

/* Fill vectors x and y with constant values.
 * 用常数值填充向量 x 和 y。 */
void initialize_vectors(long N, double x[], double y[], double x_value, double y_value)
{
    for (long i = 0; i < N; i++) {
        x[i] = x_value;
        y[i] = y_value;
    }
}

/* Compute the vector operation d[i] = a*x[i] + y[i].
 * 逐元素计算向量运算 d[i] = a*x[i] + y[i]。 */
void compute_vector_sum(long N, double a, const double x[], const double y[], double d[])
{
    for (long i = 0; i < N; i++) {
        d[i] = a * x[i] + y[i];
    }
}

/* Check whether every element of d is equal to expected within a tolerance.
 * 检查 d 的每一个元素是否在容差范围内等于期望值。 */
int test_ok(long N, const double d[], double expected)
{
    for (long i = 0; i < N; i++) {
        /* Floating-point numbers should not be compared with ==.
         * 浮点数不应该直接用 == 比较。 */
        if (fabs(d[i] - expected) > TOLERANCE) {
            printf("Wrong value at i = %ld: d[i] = %.17g, expected = %.17g\n",
                   i, d[i], expected);
            return 0;
        }
    }

    return 1;
}

/* Specific test requested in the homework: a = 3, x = 0.1, y = 7.1.
 * 作业要求的特定测试：a = 3, x = 0.1, y = 7.1。 */
void test_specific_values(void)
{
    double a = 3.0;
    double x = 0.1;
    double y = 7.1;
    double result = a * x + y;

    printf("\nSpecific test:\n");
    printf("a = %.17g, x = %.17g, y = %.17g\n", a, x, y);
    printf("Computed a*x + y = %.17g\n", result);
    printf("Mathematical expected value = 7.4\n");
    printf("Absolute error from 7.4 = %.17g\n", fabs(result - 7.4));

    /* This exact comparison may fail because of round-off error.
     * 由于舍入误差，下面的精确比较可能失败。 */
    printf("Exact comparison result == 7.4: %s\n", (result == 7.4) ? "true" : "false");

    /* This tolerance comparison is the correct numerical test.
     * 使用容差比较才是正确的数值测试方法。 */
    printf("Tolerance comparison: %s\n", (fabs(result - 7.4) <= TOLERANCE) ? "PASSED" : "FAILED");
}

int main(int argc, char *argv[])
{
    if (argc != 5) {
        printf("Usage: %s N a x y\n", argv[0]);
        printf("Example: %s 1000000 3 0.1 7.1\n", argv[0]);
        return EXIT_FAILURE;
    }

    long N = parse_positive_long(argv[1]);
    double a = parse_double(argv[2], "a");
    double x_value = parse_double(argv[3], "x");
    double y_value = parse_double(argv[4], "y");

    /* Allocate vectors on the heap.
     * 在堆内存上分配向量。 */
    double *x = allocate_vector(N, "x");
    double *y = allocate_vector(N, "y");
    double *d = allocate_vector(N, "d");

    initialize_vectors(N, x, y, x_value, y_value);
    compute_vector_sum(N, a, x, y, d);

    double expected = a * x_value + y_value;
    int correct = test_ok(N, d, expected);

    printf("Dynamic-memory version\n");
    printf("N = %ld\n", N);
    printf("a = %.17g, x = %.17g, y = %.17g\n", a, x_value, y_value);
    printf("Expected a*x + y = %.17g\n", expected);
    printf("Memory used by three double vectors = %.3f GB\n",
           3.0 * (double)N * sizeof(double) / 1.0e9);
    printf("Test: %s\n", correct ? "PASSED" : "FAILED");

    /* Print all entries only for small N to avoid huge terminal output.
     * 只在 N 较小时打印所有元素，避免终端输出过大。 */
    if (N <= 20) {
        for (long i = 0; i < N; i++) {
            printf("d[%ld] = %.17g\n", i, d[i]);
        }
    }

    test_specific_values();

    /* Always release heap memory after use.
     * 使用完堆内存后一定要释放。 */
    free(x);
    free(y);
    free(d);

    return correct ? EXIT_SUCCESS : EXIT_FAILURE;
}
