# HomeWork 03 - Matrix Multiplication in C

This homework implements the matrix multiplication

```text
C = A B
C_ij = sum_k A_ik B_kj,   i,j,k = 0, ..., N-1
```

The command-line inputs are `a`, `b`, `N`, and `fileout`.
The matrices `A` and `B` are initialized as constant matrices:

```text
A_ij = a
B_ij = b
```

Therefore, every element of the output matrix is expected to be

```text
C_ij = N * a * b
```

This follows from the summation over `k`: each entry of `C` is the sum of `N` identical terms `a*b`.

> Note: if the assignment text says to check `C = a*b`, that is only true for a different matrix definition, for example scaled identity matrices. For the present code, where all entries of `A` and `B` are constants, the mathematically correct value is `N*a*b`.

## Files

- `hw03_matmul.c`: C source code for the matrix multiplication homework.
- `out4.txt`: sample output matrix for `N = 4`.
- `out100.txt`: sample output matrix for `N = 100`.
- `out300.txt`: sample output matrix for `N = 300`.
- `README.md`: this explanation file.

## Compilation

Recommended compilation command:

```bash
gcc -std=c99 -Wall -Wextra -O2 hw03_matmul.c -o hw03_matmul -lm
```

The source file uses `clock_gettime()` for benchmarking. On some systems, this requires the POSIX feature macro `_POSIX_C_SOURCE`, which has been added at the top of the checked source file.

## Running the program

The program expects four command-line arguments:

```bash
./hw03_matmul a b N fileout
```

Example:

```bash
./hw03_matmul 2.0 3.0 4 out4.txt
```

For this example:

```text
a = 2.0
b = 3.0
N = 4
expected C_ij = N*a*b = 4*2*3 = 24
```

So every element of `out4.txt` should be `24.000000`.

## Correctness check

The code checks correctness using the minimum and maximum values of matrix `C`:

```text
min(C) == expected
max(C) == expected
```

This is faster than printing or manually comparing every element. Since all correct entries should be the same, if both the minimum and maximum are equal to the expected value, all entries are correct.

The program uses a floating-point tolerance instead of exact comparison:

```text
fabs(value - expected) <= tol
```

This is safer because floating-point arithmetic can introduce small rounding errors.

## Loop orderings benchmarked

The program benchmarks all six possible loop orderings:

```text
ijk
ikj
jik
jki
kij
kji
```

Each version computes the same mathematical result, but the execution time can be different because C stores two-dimensional arrays in row-major order. Accessing consecutive memory locations is usually faster because it improves cache locality.

In row-major storage, the index expression is:

```text
M[i*N + j]
```

Therefore, accesses that vary `j` in the innermost loop are usually cache-friendly. This is why `ikj` and `kij` are often among the fastest loop orderings.

## Example benchmark results

On my test run, using `a = 2.0`, `b = 3.0`, and `N = 300`, the timing result was approximately:

```text
ijk : 0.018064 s
ikj : 0.015381 s
jik : 0.016721 s
jki : 0.030200 s
kij : 0.013778 s
kji : 0.034069 s
```

In this run, `kij` was the fastest. The exact fastest ordering may vary depending on the machine, compiler, optimization flags, CPU cache, and current system load, but orderings with contiguous memory access in the inner loop are generally faster.

## Memory usage

The program allocates three `N x N` matrices of type `double`:

```text
A, B, C
```

The total memory requirement is approximately:

```text
3 * N * N * 8 bytes
```

For example:

```text
N = 100   -> about 0.24 MB
N = 300   -> about 2.16 MB
N = 1000  -> about 24 MB
N = 10000 -> about 2.4 GB
```

Very large values of `N` may fail if the machine does not have enough available memory.

## Summary

The implementation is correct for the interpretation where `A` and `B` are full constant matrices. In that case, every element of `C` must be `N*a*b`, not just `a*b`. The program also saves the final matrix to the requested output file and benchmarks all six loop orderings.
