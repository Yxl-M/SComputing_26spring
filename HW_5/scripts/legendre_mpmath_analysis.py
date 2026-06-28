#!/usr/bin/env python3
"""
HomeWork 05 - mpmath reference and plots.

This script computes ordinary Legendre polynomials P_l(x) with:
  1. double-like forward recurrence using Python floats;
  2. high-precision reference using mpmath with 100 decimal digits;
  3. Miller-style backward recurrence experiment.

本脚本使用 mpmath 的 100 位十进制精度作为参考值，并生成误差 CSV 和图像。
"""

from pathlib import Path
import csv
import math

import mpmath as mp
import matplotlib.pyplot as plt

mp.mp.dps = 100

LMAX = 50
L_BACK = 80
XS = [0.1, 0.5, 0.9, 0.99]
ROOT = Path(__file__).resolve().parents[1]
RESULTS_DIR = ROOT / "results"
PLOTS_DIR = ROOT / "plots"
RESULTS_DIR.mkdir(exist_ok=True)
PLOTS_DIR.mkdir(exist_ok=True)


def legendre_forward_float(x: float, lmax: int):
    """Forward Bonnet recurrence in double-like Python float arithmetic."""
    P = [0.0] * (lmax + 1)
    P[0] = 1.0
    if lmax >= 1:
        P[1] = x
    for l in range(1, lmax):
        P[l + 1] = ((2.0 * l + 1.0) * x * P[l] - l * P[l - 1]) / (l + 1.0)
    return P


def legendre_reference_mpmath(x: float, lmax: int):
    """High-precision reference recurrence with mpmath."""
    xm = mp.mpf(str(x))
    P = [mp.mpf("0")] * (lmax + 1)
    P[0] = mp.mpf("1")
    if lmax >= 1:
        P[1] = xm
    for l in range(1, lmax):
        P[l + 1] = ((2 * l + 1) * xm * P[l] - l * P[l - 1]) / (l + 1)
    return P


def legendre_backward_experiment_float(x: float, lmax: int, L: int):
    """Miller-style backward experiment with arbitrary terminal values."""
    if L <= lmax:
        raise ValueError("L must be larger than lmax")
    Q = [0.0] * (L + 2)
    Q[L + 1] = 0.0
    Q[L] = 1.0
    for l in range(L, 0, -1):
        Q[l - 1] = ((2.0 * l + 1.0) * x * Q[l] - (l + 1.0) * Q[l + 1]) / l
    scale = 1.0 / Q[0]
    return [scale * Q[l] for l in range(lmax + 1)]


def rel_error(value, reference):
    abs_err = abs(mp.mpf(value) - reference)
    if abs(reference) > mp.mpf("1e-90"):
        return abs_err / abs(reference)
    return abs_err


def main():
    rows = []
    for x in XS:
        pref = legendre_reference_mpmath(x, LMAX)
        pf = legendre_forward_float(x, LMAX)
        pb = legendre_backward_experiment_float(x, LMAX, L_BACK)
        for l in range(LMAX + 1):
            abs_f = abs(mp.mpf(pf[l]) - pref[l])
            abs_b = abs(mp.mpf(pb[l]) - pref[l])
            rel_f = rel_error(pf[l], pref[l])
            rel_b = rel_error(pb[l], pref[l])
            rows.append({
                "x": x,
                "l": l,
                "P_reference": mp.nstr(pref[l], 40),
                "P_forward": f"{pf[l]:.17e}",
                "P_backward": f"{pb[l]:.17e}",
                "abs_err_forward": mp.nstr(abs_f, 30),
                "rel_err_forward": mp.nstr(rel_f, 30),
                "abs_err_backward": mp.nstr(abs_b, 30),
                "rel_err_backward": mp.nstr(rel_b, 30),
            })

    csv_file = RESULTS_DIR / "legendre_errors_mpmath.csv"
    with csv_file.open("w", newline="") as f:
        writer = csv.DictWriter(f, fieldnames=list(rows[0].keys()))
        writer.writeheader()
        writer.writerows(rows)

    for x in XS:
        subset = [r for r in rows if float(r["x"]) == x]
        lvals = [int(r["l"]) for r in subset]
        rel_f = [float(r["rel_err_forward"]) for r in subset]
        rel_b = [float(r["rel_err_backward"]) for r in subset]
        abs_f = [float(r["abs_err_forward"]) for r in subset]
        abs_b = [float(r["abs_err_backward"]) for r in subset]

        plt.figure()
        plt.semilogy(lvals, rel_f, marker="o", label="forward recurrence")
        plt.semilogy(lvals, rel_b, marker="s", label="backward experiment")
        plt.xlabel("degree l")
        plt.ylabel("relative error")
        plt.title(f"Relative error for x = {x}")
        plt.grid(True, which="both")
        plt.legend()
        plt.tight_layout()
        plt.savefig(PLOTS_DIR / f"relative_error_x_{str(x).replace('.', 'p')}.png", dpi=160)
        plt.close()

        plt.figure()
        plt.semilogy(lvals, abs_f, marker="o", label="forward recurrence")
        plt.semilogy(lvals, abs_b, marker="s", label="backward experiment")
        plt.xlabel("degree l")
        plt.ylabel("absolute error")
        plt.title(f"Absolute error for x = {x}")
        plt.grid(True, which="both")
        plt.legend()
        plt.tight_layout()
        plt.savefig(PLOTS_DIR / f"absolute_error_x_{str(x).replace('.', 'p')}.png", dpi=160)
        plt.close()

    print(f"Wrote {csv_file}")
    print(f"Wrote plots to {PLOTS_DIR}")


if __name__ == "__main__":
    main()
