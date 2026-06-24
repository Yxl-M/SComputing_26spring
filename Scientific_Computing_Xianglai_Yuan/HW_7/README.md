# HomeWork 07 - Random Numbers, Monte Carlo Methods, and Empirical Distributions

This homework studies basic random-number simulations and Monte Carlo methods using Python.  The solution contains five exercises:

1. Coin tosses and the Law of Large Numbers
2. Monte Carlo estimation of $\pi$
3. Change of variables: $Y = U^2$
4. Inverse transform sampling for the exponential distribution
5. Empirical CDF for the exponential sample

The main solution code is in:

```text
HomeWork07_solution.py
```

A LaTeX report is also included:

```text
HomeWork07_solution.tex
```

The generated figures are:

```text
ex1_coin_lln.png
ex2_pi_error.png
ex3_y_equals_u2.png
ex4_exp_pdf.png
ex5_exp_ecdf.png
```

## Requirements

The code uses Python 3 with the following packages:

```bash
pip install numpy matplotlib
```

## How to Run

From this directory, run:

```bash
python3 HomeWork07_solution.py
```

The script prints numerical results in the terminal and produces the five PNG figures listed above.

If the code is run on a server or non-interactive environment, use:

```bash
MPLBACKEND=Agg python3 HomeWork07_solution.py
```

In that case, a harmless warning may appear because the script calls `plt.show()` at the end. The figures are still saved correctly.

## Exercise 1 - Coin Tosses and LLN

The code generates $100000$ independent fair coin tosses, with heads encoded as `1` and tails encoded as `0`. The running fraction of heads is

$$
\hat p_n = \frac{1}{n}\sum_{i=1}^n X_i.
$$

Since the coin is fair, the theoretical value is

$$
E[X_i] = 0.5.
$$

The output shows that the running fraction fluctuates for small $n$, but becomes closer to $0.5$ as $n$ increases. This agrees with the Law of Large Numbers.

## Exercise 2 - Monte Carlo Estimate of $\pi$

The code estimates $\pi$ by generating random points in the unit square $[0,1]^2$. A point is inside the quarter unit circle if

$$
x^2 + y^2 \leq 1.
$$

The estimator is

$$
\hat\pi_N = 4\frac{N_{\mathrm{in}}}{N}.
$$

Two independent RNG streams are used: one for the $x$ coordinates and one for the $y$ coordinates. The code tests several sample sizes:

```text
N = 100, 1000, 10000, 100000, 1000000
```

The error is random and therefore not strictly monotonic, but its typical size decreases as $N$ increases. The expected Monte Carlo scaling is

$$
|\hat\pi_N - \pi| = O(N^{-1/2}).
$$

## Exercise 3 - Change of Variables: $Y = U^2$

The code generates

$$
U \sim \mathrm{Uniform}(0,1)
$$

and transforms it into

$$
Y = U^2.
$$

For $0<y<1$, the analytic density is obtained by change of variables:

$$
f_Y(y) = \frac{1}{2\sqrt{y}}.
$$

The histogram of the simulated values follows this analytic density. The density is large near $y=0$, which is expected because the transformation compresses more probability mass near zero.

## Exercise 4 - Inverse Transform Exponential

The code uses inverse transform sampling to generate an exponential random variable with rate

$$
\lambda = 1.5.
$$

Starting from

$$
F(y)=1-e^{-\lambda y},
$$

and setting $F(y)=U$, the inverse formula is

$$
Y = -\frac{\ln(1-U)}{\lambda}.
$$

The theoretical PDF is

$$
f_Y(y) = \lambda e^{-\lambda y}, \qquad y\geq 0.
$$

The sample histogram agrees with the exact PDF. The sample mean is close to the theoretical mean

$$
E[Y] = \frac{1}{\lambda} = \frac{1}{1.5} \approx 0.6667.
$$

## Exercise 5 - Empirical CDF

For the exponential sample from Exercise 4, the empirical CDF is

$$
\hat F_M(y) = \frac{1}{M}\sum_{i=1}^M \mathbf{1}\{Y_i \leq y\}.
$$

The exact CDF is

$$
F(y)=1-e^{-1.5y}.
$$

The empirical CDF curve is close to the theoretical CDF. The small differences are due to finite-sample randomness.

## Correctness Check

The submitted solution is mathematically and computationally correct:

- Exercise 1 correctly demonstrates convergence of the running heads fraction to $0.5$.
- Exercise 2 correctly uses two independent RNG streams and the quarter-circle estimator for $\pi$.
- Exercise 3 correctly compares the histogram of $Y=U^2$ with $f_Y(y)=1/(2\sqrt{y})$.
- Exercise 4 correctly implements inverse transform sampling for the exponential distribution.
- Exercise 5 correctly compares the empirical CDF with the exact exponential CDF.

The only minor technical note is that `plt.show()` may produce a warning in a non-interactive backend, but this does not affect the saved figures or numerical results.
