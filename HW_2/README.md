# HomeWork 02: Vector Sum in C

## 1. Objective

The objective of this homework is to implement the vector operation

\[
\vec d = a\vec x + \vec y,
\]

where `a` is a scalar and `d`, `x`, and `y` are vectors of dimension `N`.
In this assignment, all elements of vector `x` have the same value, and all elements of vector `y` have the same value.
The values of `N`, `a`, `x`, and `y` are provided from the command line.

The required values of `N` to test are:

```text
N = 10
N = 10^6
N = 10^8
```

The specific numerical test requested in the homework is:

```text
a = 3, x = 0.1, y = 7.1
```

Mathematically,

\[
3 \times 0.1 + 7.1 = 7.4.
\]

However, this case is also useful for observing floating-point round-off errors in C.

## 2. Files included

This solution contains two C programs:

- `sum2vec_static_commented.c`: a stack-memory version using variable-length arrays.
- `sum2vec_dynamic_commented.c`: a dynamic-memory version using `malloc()` and `free()`.

The two programs compute the same vector operation. The main difference is the memory allocation strategy.
The code files contain bilingual comments in English and Chinese to make the implementation easier to understand.

## 3. Compilation

Compile the stack-memory version with:

```bash
gcc -std=c99 -Wall -Wextra sum2vec_static_commented.c -o sum2vec_static.x -lm
```

Compile the dynamic-memory version with:

```bash
gcc -std=c99 -Wall -Wextra sum2vec_dynamic_commented.c -o sum2vec_dynamic.x -lm
```

The option `-lm` links the math library, which is needed because the programs use `fabs()` for floating-point comparison.

## 4. How to run

The command-line format is:

```bash
./program_name N a x y
```

For example, using the dynamic-memory version:

```bash
./sum2vec_dynamic.x 10 3 0.1 7.1
./sum2vec_dynamic.x 1000000 3 0.1 7.1
./sum2vec_dynamic.x 100000000 3 0.1 7.1
```

Using the stack-memory version:

```bash
./sum2vec_static.x 10 3 0.1 7.1
./sum2vec_static.x 1000000 3 0.1 7.1
```

For large `N`, the stack-memory version may fail because stack memory is limited.

## 5. Algorithm

The program follows these steps:

1. Read `N`, `a`, `x`, and `y` from the command line.
2. Allocate the vectors `x`, `y`, and `d`.
3. Initialize all elements of `x` with the same input value.
4. Initialize all elements of `y` with the same input value.
5. Compute each element of `d` using

   ```c
   d[i] = a * x[i] + y[i];
   ```

6. Compute the expected value

   ```c
   expected = a * x_value + y_value;
   ```

7. Check every element of `d` against `expected` using a tolerance.

## 6. Correctness test

The program checks correctness with a tolerance instead of exact equality:

```c
fabs(d[i] - expected) <= 1e-12
```

This is necessary because floating-point arithmetic is not exact for most decimal numbers.
For example, decimal values such as `0.1`, `7.1`, and `7.4` cannot usually be represented exactly in binary floating-point format.

Therefore, a comparison such as

```c
result == 7.4
```

is not reliable. A tolerance-based comparison is the correct numerical approach.

## 7. Required test: `a = 3`, `x = 0.1`, `y = 7.1`

For the required values,

```text
a = 3
x = 0.1
y = 7.1
```

the mathematical result is:

```text
3*0.1 + 7.1 = 7.4
```

In C using `double`, the computed value may be printed as something close to:

```text
7.3999999999999995
```

This is not a logical error in the program. It is a normal consequence of binary floating-point representation.
For this reason, the program includes a specific test function that prints:

- the computed value of `a*x + y`,
- the absolute error with respect to `7.4`,
- whether exact comparison with `7.4` succeeds,
- whether tolerance comparison succeeds.

The expected behavior is that exact comparison may fail, while tolerance comparison should pass.

## 8. Memory discussion

### Case `N = 10`

This case is very small. Both the stack-memory version and the dynamic-memory version should run without problems.

### Case `N = 10^6`

For `N = 10^6`, the program needs three arrays of type `double`:

```text
x, y, d
```

Since one `double` usually uses 8 bytes, the approximate memory requirement is:

\[
3 \times 10^6 \times 8 \text{ bytes} = 24 \text{ MB}.
\]

The dynamic-memory version should normally work. The stack-memory version may fail with a segmentation fault because stack memory is often much smaller than heap memory.

### Case `N = 10^8`

For `N = 10^8`, the approximate memory requirement is:

\[
3 \times 10^8 \times 8 \text{ bytes} = 2.4 \text{ GB}.
\]

This may fail if the machine does not have enough available memory. Even if the allocation succeeds, the program may be slower because it has to initialize and process very large arrays.

The dynamic-memory version checks whether `malloc()` returns `NULL`. If the allocation fails, the program prints an error message instead of continuing with invalid memory.

## 9. Example output

Example command:

```bash
./sum2vec_dynamic.x 10 3 0.1 7.1
```

Possible output:

```text
Dynamic-memory version
N = 10
a = 3, x = 0.10000000000000001, y = 7.0999999999999996
Expected a*x + y = 7.3999999999999995
Memory used by three double vectors = 0.000 GB
Test: PASSED
d[0] = 7.3999999999999995
d[1] = 7.3999999999999995
...
Specific test:
a = 3, x = 0.10000000000000001, y = 7.0999999999999996
Computed a*x + y = 7.3999999999999995
Mathematical expected value = 7.4
Absolute error from 7.4 = 8.8817841970012523e-16
Exact comparison result == 7.4: false
Tolerance comparison: PASSED
```

The exact printed floating-point values may vary slightly depending on the system and compiler, but the tolerance-based test should pass.

## 10. Conclusion

The program correctly computes the vector operation

\[
\vec d = a\vec x + \vec y.
\]

The result is checked by verifying every element of `d` against the expected value using a numerical tolerance.

The main issue for large `N` is memory allocation. The stack-memory version is simple but can fail for large arrays. The dynamic-memory version is more appropriate for large vectors, although `N = 10^8` may still require too much memory on some machines.

The second important issue is floating-point precision. Although `3*0.1 + 7.1` is mathematically equal to `7.4`, the value stored in a `double` may not be exactly `7.4`. Therefore, exact comparison using `==` should be avoided, and tolerance-based comparison should be used instead.
