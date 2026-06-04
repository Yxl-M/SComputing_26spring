"""
HomeWork 07 solution
Topics: Law of Large Numbers, Monte Carlo estimation, variable transformation,
inverse transform sampling, and empirical CDF.
"""
import math
import numpy as np
import matplotlib.pyplot as plt

# ---------------- Exercise 1: Coin tosses and LLN ----------------
rng = np.random.default_rng(20260511)
N = 100_000
tosses = rng.integers(0, 2, size=N)  # 1=heads, 0=tails
running_fraction_heads = np.cumsum(tosses) / np.arange(1, N + 1)

print("Exercise 1: running fraction of heads")
for n in [10, 100, 1_000, 10_000, 100_000]:
    frac = running_fraction_heads[n - 1]
    print(f"n={n:>7,d}, fraction={frac:.6f}, deviation={frac - 0.5:+.6f}")

plt.figure()
plt.plot(np.arange(1, N + 1), running_fraction_heads)
plt.axhline(0.5, linestyle="--")
plt.xscale("log")
plt.xlabel("Number of tosses n")
plt.ylabel("Running fraction of heads")
plt.title("LLN for fair coin tosses")
plt.tight_layout()
plt.savefig("ex1_coin_lln.png", dpi=200)

# ---------------- Exercise 2: Monte Carlo estimate of pi ----------------
# Two independent RNG streams are used: one for x-coordinates and one for y-coordinates.
seed_sequence = np.random.SeedSequence(12345)
seq_x, seq_y = seed_sequence.spawn(2)
rng_x = np.random.default_rng(seq_x)
rng_y = np.random.default_rng(seq_y)

sample_sizes = np.array([100, 1_000, 10_000, 100_000, 1_000_000])
maxN = int(sample_sizes.max())
x = rng_x.random(maxN)
y = rng_y.random(maxN)
inside_cumsum = np.cumsum(x*x + y*y <= 1.0)

print("\nExercise 2: Monte Carlo estimate of pi")
pi_estimates = []
errors = []
for n in sample_sizes:
    inside = inside_cumsum[n - 1]
    pi_hat = 4 * inside / n
    error = abs(pi_hat - math.pi)
    pi_estimates.append(pi_hat)
    errors.append(error)
    print(f"N={n:>9,d}, inside={inside:>9,d}, pi_hat={pi_hat:.8f}, error={error:.8f}")

plt.figure()
plt.loglog(sample_sizes, errors, marker="o", label="absolute error")
reference = errors[0] * (sample_sizes / sample_sizes[0])**(-0.5)
plt.loglog(sample_sizes, reference, linestyle="--", label="reference N^{-1/2}")
plt.xlabel("Sample size N")
plt.ylabel("|pi_hat - pi|")
plt.title("Monte Carlo error for pi")
plt.legend()
plt.tight_layout()
plt.savefig("ex2_pi_error.png", dpi=200)

# ---------------- Exercise 3: Change of variables Y = U^2 ----------------
rng3 = np.random.default_rng(222)
M = 200_000
U = rng3.random(M)
Y = U**2

y_grid = np.linspace(1e-4, 1.0, 500)
fY = 1 / (2 * np.sqrt(y_grid))

plt.figure()
plt.hist(Y, bins=80, density=True, alpha=0.45, label="simulation histogram")
plt.plot(y_grid, fY, label="analytic density")
plt.ylim(0, 8)
plt.xlabel("y")
plt.ylabel("density")
plt.title("Transformation Y = U^2")
plt.legend()
plt.tight_layout()
plt.savefig("ex3_y_equals_u2.png", dpi=200)

print("\nExercise 3: The histogram follows f_Y(y)=1/(2 sqrt(y)) on 0<y<1.")

# ---------------- Exercise 4: Inverse transform exponential ----------------
lambda_value = 1.5
rng4 = np.random.default_rng(333)
U = rng4.random(M)
Yexp = -np.log(1 - U) / lambda_value

x_grid = np.linspace(0, 6, 500)
pdf = lambda_value * np.exp(-lambda_value * x_grid)

plt.figure()
plt.hist(Yexp, bins=100, range=(0, 6), density=True, alpha=0.45, label="simulation histogram")
plt.plot(x_grid, pdf, label="exact PDF")
plt.xlabel("y")
plt.ylabel("density")
plt.title("Inverse-transform exponential, lambda = 1.5")
plt.legend()
plt.tight_layout()
plt.savefig("ex4_exp_pdf.png", dpi=200)

print("Exercise 4: sample mean =", np.mean(Yexp), "; theoretical mean =", 1/lambda_value)

# ---------------- Exercise 5: Empirical CDF ----------------
ys = np.sort(Yexp)
ecdf = np.arange(1, M + 1) / M
cdf_exact = 1 - np.exp(-lambda_value * x_grid)

plt.figure()
idx = np.linspace(0, M - 1, 3000, dtype=int)
plt.step(ys[idx], ecdf[idx], where="post", label="empirical CDF")
plt.plot(x_grid, cdf_exact, linestyle="--", label="exact CDF")
plt.xlim(0, 6)
plt.ylim(0, 1.01)
plt.xlabel("y")
plt.ylabel("CDF")
plt.title("Empirical CDF for exponential sample")
plt.legend()
plt.tight_layout()
plt.savefig("ex5_exp_ecdf.png", dpi=200)

print("Exercise 5: The empirical CDF is close to F(y)=1-exp(-1.5y).")
plt.show()
