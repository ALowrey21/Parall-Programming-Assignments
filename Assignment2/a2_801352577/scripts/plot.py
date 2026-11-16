"""
Plots for Assignment 2, Part A (pthreads).

Run from the project's top folder:
    python3 scripts/plot.py

Reads:
    data/pth_oned_medians.csv
    data/pth_twod_medians.csv
Writes:
    plots/pth_oned_speedup.png      (Part 2: 1D speedup, both sizes, both tile sizes)
    plots/pth_twod_vs_oned.png      (Part 3: 2D square grid vs 1D b*, one axis per size)
"""

import os

import matplotlib
matplotlib.use("Agg")  # draw to files only; no window needed (works on the cluster)
import matplotlib.pyplot as plt
import pandas as pd

DATA_DIR = "data"
PLOT_DIR = "plots"
SIZES = [1024, 2048]
P_TICKS = [1, 2, 4, 8, 12, 18, 36]


def setup_log_p_axis(ax):
    """Log base-2 x-axis labeled with the actual p values, plus the ideal S = p line."""
    ax.plot(P_TICKS, P_TICKS, linestyle="--", color="gray", label="Ideal (S = p)")
    ax.set_xscale("log", base=2)
    ax.set_xticks(P_TICKS)
    ax.set_xticklabels([str(p) for p in P_TICKS])
    ax.minorticks_off()
    ax.set_xlabel("p (threads)")
    ax.set_ylabel("Speedup S(p)")
    ax.grid(True, alpha=0.3)


def plot_oned(oned, b_star):
    """Part 2: S(p) vs p for both matrix sizes and both tile sizes (b = 0 and b = b*)."""
    fig, ax = plt.subplots(figsize=(7, 5))

    # if b* is 0, the "b = 0" and "b = b*" runs are the same tile size, so plot it once
    tile_sizes = [0] if b_star == 0 else [0, b_star]

    for size in SIZES:
        for b in tile_sizes:
            rows = oned[(oned["M"] == size) & (oned["b"] == b)].sort_values("p")
            ax.plot(rows["p"], rows["Speed_Up"], marker="o",
                    label=f"{size}, b = {b}")

    setup_log_p_axis(ax)
    ax.set_title("One-dimensional partition: speedup vs. threads")
    ax.legend()
    fig.tight_layout()
    fig.savefig(os.path.join(PLOT_DIR, "pth_oned_speedup.png"), dpi=200)
    plt.close(fig)


def plot_twod_vs_oned(oned, twod, b_star):
    """Part 3: 2D square grid vs 1D at b = b*, one axis per matrix size."""
    fig, axes = plt.subplots(1, 2, figsize=(12, 5))

    for ax, size in zip(axes, SIZES):
        # 2D square grid only: drop the 1 x 36 column grid (p = 36, pr = 1)
        # and the non-divisible run (M = 1000), which are not on this curve
        square = twod[(twod["M"] == size)
                      & ~((twod["p"] == 36) & (twod["pr"] == 1))]
        square = square.sort_values("p")

        oned_bstar = oned[(oned["M"] == size) & (oned["b"] == b_star)].sort_values("p")

        ax.plot(square["p"], square["Speed_Up"], marker="s",
                label=f"2D square grid, b = {b_star}")
        ax.plot(oned_bstar["p"], oned_bstar["Speed_Up"], marker="o",
                label=f"1D, b = {b_star}")

        setup_log_p_axis(ax)
        ax.set_title(f"M = K = N = {size}")
        ax.legend()

    fig.suptitle("Two-dimensional vs. one-dimensional partition")
    fig.tight_layout()
    fig.savefig(os.path.join(PLOT_DIR, "pth_twod_vs_oned.png"), dpi=200)
    plt.close(fig)


def main():
    os.makedirs(PLOT_DIR, exist_ok=True)

    oned = pd.read_csv(os.path.join(DATA_DIR, "pth_oned_medians.csv"))
    twod = pd.read_csv(os.path.join(DATA_DIR, "pth_twod_medians.csv"))

    # if b* = 0, the 1D file has the b = 0 rows twice; keep one of each
    oned = oned.drop_duplicates(subset=["M", "b", "p"])

    # every 2D run uses b*, so read it from there
    b_star = int(twod["b"].iloc[0])

    plot_oned(oned, b_star)
    plot_twod_vs_oned(oned, twod, b_star)
    print(f"Saved plots to {PLOT_DIR}/ (b* = {b_star})")


if __name__ == "__main__":
    main()