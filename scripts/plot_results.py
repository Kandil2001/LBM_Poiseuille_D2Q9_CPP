import argparse
import math
from pathlib import Path

import matplotlib.pyplot as plt
import pandas as pd


RESULTS_DIR = Path("results")
PROFILE_FILE = RESULTS_DIR / "centerline_profile.csv"
CONVERGENCE_FILE = RESULTS_DIR / "convergence.csv"


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Generate LBM validation plots and optionally enforce an analytical-error limit."
    )
    parser.add_argument(
        "--max-relative-linf",
        type=float,
        default=None,
        help="Fail when the relative L-infinity velocity-profile error exceeds this value.",
    )
    return parser.parse_args()


def require_file(path: Path) -> None:
    if not path.exists():
        raise FileNotFoundError(
            f"Missing {path}. Run the solver first, for example: ./lbm_channel"
        )


def plot_velocity_profile(profile: pd.DataFrame) -> None:
    plt.figure()
    plt.plot(profile["lbm_ux"], profile["y"], "o", label="LBM")
    plt.plot(profile["analytical_ux"], profile["y"], "-", label="Analytical")
    plt.xlabel("Streamwise velocity, ux")
    plt.ylabel("Wall-normal lattice coordinate, y")
    plt.title("Poiseuille velocity profile")
    plt.legend()
    plt.grid(True)
    plt.tight_layout()
    plt.savefig(RESULTS_DIR / "velocity_profile.png", dpi=200)
    plt.close()


def plot_convergence(convergence: pd.DataFrame) -> None:
    plt.figure()
    plt.semilogy(convergence["step"], convergence["max_change"])
    plt.xlabel("Time step")
    plt.ylabel("Maximum change in ux")
    plt.title("Convergence history")
    plt.grid(True)
    plt.tight_layout()
    plt.savefig(RESULTS_DIR / "convergence.png", dpi=200)
    plt.close()


def validation_metrics(profile: pd.DataFrame) -> tuple[float, float]:
    error = (profile["lbm_ux"] - profile["analytical_ux"]).abs()
    max_absolute_error = float(error.max())
    reference = float(profile["analytical_ux"].abs().max())
    relative_linf = max_absolute_error / reference if reference > 0.0 else float("nan")
    return max_absolute_error, relative_linf


def print_validation_summary(max_absolute_error: float, relative_linf: float) -> None:
    print("Validation summary")
    print(f"  max absolute error  : {max_absolute_error:.6e}")
    print(f"  relative L_inf error: {relative_linf:.6e}")


def enforce_error_limit(relative_linf: float, limit: float | None) -> None:
    if limit is None:
        return
    if limit <= 0.0:
        raise ValueError("--max-relative-linf must be greater than zero.")
    if not math.isfinite(relative_linf):
        raise SystemExit("Validation failed: the relative L_inf error is not finite.")
    if relative_linf > limit:
        raise SystemExit(
            "Validation failed: relative L_inf error "
            f"{relative_linf:.6e} exceeds the limit {limit:.6e}."
        )
    print(f"Validation passed: relative L_inf error <= {limit:.6e}")


def main() -> None:
    args = parse_args()
    require_file(PROFILE_FILE)
    require_file(CONVERGENCE_FILE)

    profile = pd.read_csv(PROFILE_FILE)
    convergence = pd.read_csv(CONVERGENCE_FILE)

    plot_velocity_profile(profile)
    plot_convergence(convergence)
    max_absolute_error, relative_linf = validation_metrics(profile)
    print_validation_summary(max_absolute_error, relative_linf)
    enforce_error_limit(relative_linf, args.max_relative_linf)
    print(f"Saved plots in {RESULTS_DIR}/")


if __name__ == "__main__":
    main()
