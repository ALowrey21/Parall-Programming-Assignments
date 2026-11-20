"""
Plots and report tables for Assignment 2 (Part A: pthreads, Part B: OpenMP).

Run from the project's top folder, after the C runs have finished:
    python3 scripts/plot.py

For each part whose CSV files exist in data/ (prefix "pth" or "omp"), it reads:
    data/<prefix>_sequential_medians.csv   (T_seq = the b = 0 median for each size)
    data/<prefix>_1D_medians.csv
    data/<prefix>_2D_medians.csv
and b* from data/b_star_holder.txt (written by the Part A sweep).

It writes:
    plots/<prefix>_1D_speedup.png     S(p) vs p, both sizes, b = 0 and b = b*
    plots/<prefix>_2D_vs_1D.png       2D square grid vs 1D at b = b*, one axis per size
    data/<prefix>_1D_table.csv        1D medians with the Speed_Up column added
    data/<prefix>_2D_table.csv        2D medians with the Speed_Up column added
and prints T_seq, b*, and T(1)/T_seq for each size.

A part whose CSV files are missing is skipped, so this works before Part B has run.
"""

import os

import matplotlib
matplotlib.use("Agg")  # draw to files only; no display needed (works on the cluster)
import matplotlib.pyplot as plt
import pandas as pd

DATA_DIR = "data"
PLOT_DIR = "plots"
SIZES = [1024, 2048]
PARTS = {"pth": "pthreads", "omp": "OpenMP"}


def data_path(name):
    return os.path.join(DATA_DIR, name)


def read_b_star(twod):
    """b* from the Part A sweep's file; falls back to the b used in the 2D runs."""
    path = data_path("b_star_holder.txt")
    if os.path.exists(path):
        with open(path) as f:
            return int(f.read().strip())
    return int(twod["b"].iloc[0])


def find_tseq(seq):
    """T_seq for each size: the median of the sequential untiled (b = 0) run."""
    tseq = {}
    for size in SIZES:
        rows = seq[(seq["M"] == size) & (seq["K"] == size)
                   & (seq["N"] == size) & (seq["b"] == 0)]
        if not rows.empty:
            tseq[size] = rows["Median"].iloc[0]
    return tseq


def add_speedup(df, tseq):
    """Speed_Up = T_seq / T(p), using the T_seq for that row's size.
    Rows with no matching T_seq (the non-divisible run) are left blank."""
    df = df.copy()
    df["Speed_Up"] = df["M"].map(tseq) / df["Median"]
    return df


def setup_speedup_axis(ax, p_values):
    """Log p axis with ticks at the measured values, linear S axis, and the S = p line."""
    ax.plot(p_values, p_values, linestyle="--", color="gray", label="Ideal (S = p)")
    ax.set_xscale("log", base=2)
    ax.set_xticks(p_values)
    ax.set_xticklabels([str(p) for p in p_values])
    ax.minorticks_off()
    ax.set_xlabel("p (threads)")
    ax.set_ylabel("Speedup S(p)")
    ax.grid(True, alpha=0.3)


def plot_1d(prefix, oned, b_star):
    """S(p) vs p for both matrix sizes and both tile sizes (b = 0 and b = b*)."""
    fig, ax = plt.subplots(figsize=(7, 5))

    # if b* is 0, "b = 0" and "b = b*" are the same runs, so draw them once
    tile_sizes = [0] if b_star == 0 else [0, b_star]

    for size in SIZES:
        for b in tile_sizes:
            rows = oned[(oned["M"] == size) & (oned["b"] == b)].sort_values("p")
            if not rows.empty:
                ax.plot(rows["p"], rows["Speed_Up"], marker="o", label=f"{size}, b = {b}")

    p_values = sorted(oned["p"].unique())
    setup_speedup_axis(ax, p_values)
    ax.set_title(f"{PARTS[prefix]}: one-dimensional partition")
    ax.legend()
    fig.tight_layout()
    fig.savefig(os.path.join(PLOT_DIR, f"{prefix}_1D_speedup.png"), dpi=200)
    plt.close(fig)


def plot_2d_vs_1d(prefix, oned, twod, b_star):
    """2D square grid vs 1D at b = b*, one axis per matrix size."""
    fig, axes = plt.subplots(1, 2, figsize=(12, 5))

    for ax, size in zip(axes, SIZES):
        # square grid only: leave out the 1 x 36 column grid
        square = twod[(twod["M"] == size) & (twod["K"] == size) & (twod["N"] == size)
                      & ~((twod["p"] == 36) & (twod["pr"] == 1))].sort_values("p")
        oned_bstar = oned[(oned["M"] == size) & (oned["b"] == b_star)].sort_values("p")

        if not square.empty:
            ax.plot(square["p"], square["Speed_Up"], marker="s",
                    label=f"2D square grid, b = {b_star}")
        if not oned_bstar.empty:
            ax.plot(oned_bstar["p"], oned_bstar["Speed_Up"], marker="o",
                    label=f"1D, b = {b_star}")

        p_values = sorted(set(square["p"]) | set(oned_bstar["p"]))
        setup_speedup_axis(ax, p_values)
        ax.set_title(f"M = K = N = {size}")
        ax.legend()

    fig.suptitle(f"{PARTS[prefix]}: two-dimensional vs. one-dimensional partition")
    fig.tight_layout()
    fig.savefig(os.path.join(PLOT_DIR, f"{prefix}_2D_vs_1D.png"), dpi=200)
    plt.close(fig)


def process_part(prefix):
    files = [data_path(f"{prefix}_sequential_medians.csv"),
             data_path(f"{prefix}_1D_medians.csv"),
             data_path(f"{prefix}_2D_medians.csv")]
    missing = [f for f in files if not os.path.exists(f)]
    if missing:
        print(f"Skipping {PARTS[prefix]}: missing {', '.join(missing)}")
        return

    seq, oned, twod = (pd.read_csv(f) for f in files)

    # if b* = 0, the b = 0 and b = b* 1D runs are the same configuration; keep one of each
    oned = oned.drop_duplicates(subset=["M", "K", "N", "b", "p"])

    b_star = read_b_star(twod)
    tseq = find_tseq(seq)

    oned = add_speedup(oned, tseq)
    twod = add_speedup(twod, tseq)

    # completed tables for the report
    oned.to_csv(data_path(f"{prefix}_1D_table.csv"), index=False)
    twod.to_csv(data_path(f"{prefix}_2D_table.csv"), index=False)

    plot_1d(prefix, oned, b_star)
    plot_2d_vs_1d(prefix, oned, twod, b_star)

    print(f"\n{PARTS[prefix]}: b* = {b_star}")
    for size in SIZES:
        if size not in tseq:
            print(f"  {size}: no T_seq found (sequential b = 0 run missing)")
            continue
        t1 = oned[(oned["M"] == size) & (oned["b"] == 0) & (oned["p"] == 1)]
        line = f"  {size}: T_seq = {tseq[size]:.6f} s"
        if not t1.empty:
            ratio = t1["Median"].iloc[0] / tseq[size]
            line += f", T(1) = {t1['Median'].iloc[0]:.6f} s, T(1)/T_seq = {ratio:.4f}"
        print(line)


def main():
    os.makedirs(PLOT_DIR, exist_ok=True)
    for prefix in PARTS:
        process_part(prefix)
    print(f"\nPlots are in {PLOT_DIR}/ and tables are in {DATA_DIR}/")


if __name__ == "__main__":
    main()