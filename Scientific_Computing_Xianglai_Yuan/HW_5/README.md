# HomeWork 05 - Stability of Legendre Polynomials

This repository contains the complete answer for **Scientific Computing - HomeWork 05**.

The goal is to study the numerical stability of computing ordinary Legendre polynomials

```text
P_l(x),   l = 0, 1, ..., 50
```

for

```text
x = 0.1, 0.5, 0.9, 0.99
```

using:

1. forward three-term recurrence in `double` precision;
2. high-precision reference values;
3. a Miller-style backward recurrence experiment.

All required explanation, compilation instructions, execution instructions, numerical results, and conclusions are included in this single `README.md` file.

---

## 1. Files

```text
HW05_Legendre_Stability/
├── README.md
├── Makefile
├── src/
│   └── legendre_stability.c
├── scripts/
│   └── legendre_mpmath_analysis.py
├── results/
│   ├── legendre_errors_c.csv
│   └── legendre_errors_mpmath.csv
└── plots/
    ├── relative_error_x_0p1.png
    ├── relative_error_x_0p5.png
    ├── relative_error_x_0p9.png
    ├── relative_error_x_0p99.png
    ├── absolute_error_x_0p1.png
    ├── absolute_error_x_0p5.png
    ├── absolute_error_x_0p9.png
    └── absolute_error_x_0p99.png
```

Main files:

- `src/legendre_stability.c`: C implementation of the forward recurrence, the backward recurrence experiment, and CSV output. The code comments are written in both English and Chinese.
- `scripts/legendre_mpmath_analysis.py`: Python script using `mpmath` with 100 decimal digits to generate high-precision references and plots.
- `results/`: CSV numerical data.
- `plots/`: relative-error and absolute-error figures.

---

## 2. Mathematical background

For order `m = 0`, the spherical harmonics are related to the ordinary Legendre polynomials by

```text
Y_{l0}(theta, phi) = sqrt((2l + 1)/(4pi)) P_l(cos(theta)).
```

Therefore, numerical errors in `P_l(x)` directly affect the computation of `Y_{l0}` up to a known normalization factor.

The ordinary Legendre polynomials satisfy Bonnet's recurrence:

```text
P_0(x) = 1
P_1(x) = x
P_{l+1}(x) = ((2l + 1)xP_l(x) - lP_{l-1}(x)) / (l + 1)
```

This is the forward recurrence used in the C code.

Solving the same recurrence for `P_{l-1}(x)` gives the backward form:

```text
P_{l-1}(x) = ((2l + 1)xP_l(x) - (l + 1)P_{l+1}(x)) / l
```

In the Miller-style experiment, the code starts from arbitrary terminal values

```text
Q_{L+1} = 0
Q_L = 1
```

then propagates backward and rescales the sequence so that

```text
Q_0 = 1.
```

In this homework, I used

```text
L = 80
lmax = 50
```

so the backward recurrence starts above the maximum degree required for comparison.

---

## 3. Compile

From the project folder, run:

```bash
make
```

or compile manually:

```bash
gcc -std=c11 -Wall -Wextra -O2 src/legendre_stability.c -lm -o legendre_stability
```

---

## 4. Run the C program

Run the default homework experiment:

```bash
make run
```

This generates:

```text
results/legendre_errors_c.csv
```

Manual usage:

```bash
./legendre_stability lmax L output_file
```

Example:

```bash
./legendre_stability 50 80 results/legendre_errors_c.csv
```

The CSV file contains:

```text
x,l,P_reference,P_forward,P_backward,abs_err_forward,rel_err_forward,abs_err_backward,rel_err_backward
```

---

## 5. Run the Python high-precision analysis

The Python script uses `mpmath` with 100 decimal digits as the high-precision reference.

Install dependencies if needed:

```bash
pip install mpmath matplotlib
```

Run:

```bash
make python
```

or manually:

```bash
python3 scripts/legendre_mpmath_analysis.py
```

This generates:

```text
results/legendre_errors_mpmath.csv
```

and the plots in the `plots/` directory.

---

## 6. Error definitions

The absolute error is defined as

```text
E_abs(l) = |P_l^num(x) - P_l^ref(x)|.
```

The relative error is defined as

```text
E_rel(l) = |P_l^num(x) - P_l^ref(x)| / |P_l^ref(x)|.
```

When the reference value is extremely close to zero, the program avoids division by a very small number and uses the absolute error instead. This prevents misleading relative-error spikes near zeros of the Legendre polynomial.

---

## 7. Numerical results

Using the Python `mpmath` script with 100 decimal digits, the maximum errors over `l = 0, ..., 50` are approximately:

| x | max relative error, forward | max absolute error, forward | max relative error, backward experiment | max absolute error, backward experiment |
|---:|---:|---:|---:|---:|
| 0.1 | 5.59e-15 | 4.95e-17 | 2.77e+2 | 2.48e+0 |
| 0.5 | 7.68e-16 | 6.16e-17 | 4.73e+1 | 7.88e+0 |
| 0.9 | 1.21e-14 | 4.50e-16 | 1.59e+0 | 1.20e-1 |
| 0.99 | 3.64e-14 | 9.82e-16 | 2.89e+0 | 2.51e-1 |

The results show that the forward recurrence stays close to double-precision roundoff error for all tested values of `x`. The backward recurrence experiment gives much larger errors because arbitrary terminal values do not generally select the ordinary Legendre polynomial sequence.

---

## 8. Interpretation

### Forward recurrence

The forward recurrence is stable for this experiment. For `x = 0.1, 0.5, 0.9, 0.99` and `l <= 50`, the computed values agree with the high-precision reference very well. The relative error is usually close to the expected double-precision roundoff level, except near polynomial zeros where relative error can become less informative.

### Backward recurrence experiment

The backward recurrence starts from arbitrary values at large degree and then rescales the result to match `P_0(x) = 1`. This normalization enforces agreement only at `l = 0`; it does not guarantee that the whole sequence agrees with the true Legendre polynomial sequence.

Miller's algorithm is useful when the desired solution is the minimal solution in the direction of backward propagation. In this ordinary Legendre polynomial test on `|x| < 1`, the arbitrary backward experiment is not a reliable method for computing `P_l(x)`.

### Connection to spherical harmonics

For `m = 0`,

```text
Y_{l0}(theta, phi) = sqrt((2l + 1)/(4pi)) P_l(cos(theta)).
```

Thus, if `x = cos(theta)`, any error in `P_l(x)` is transferred directly to `Y_{l0}` after multiplication by the normalization factor. A stable method for evaluating `P_l(x)` is therefore necessary for stable computation of the `m = 0` spherical harmonics.

---

## 9. Final conclusion

For the tested range `l <= 50`, the standard forward recurrence for ordinary Legendre polynomials is stable and accurate in double precision. The Miller-style backward recurrence with arbitrary terminal values is not accurate for this problem and should not be used as a replacement for the forward recurrence unless the stability conditions of Miller's algorithm are satisfied.

---

## 10. Clean generated files

To remove compiled files and generated outputs, run:

```bash
make clean
```
