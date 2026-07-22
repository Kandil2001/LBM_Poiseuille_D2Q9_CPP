import argparse
import math
from pathlib import Path

import matplotlib.pyplot as plt
import pandas as pd


RESULTS_DIR = Path("results")
PROFILE_FILE = RESULTS_DIR / "centerline_profile.csv"
CONVERGENCE_FILE = RESULTS_DIR / "convergence.csv"
VELOCITY_FILE = RESULTS_DIR / "velocity_field.csv"
SUMMARY_FILE = RESULTS_DIR / "verification_summary.csv"


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Generate LBM plots and optionally enforce numerical-verification limits."
    )
    parser.add_argument(
        "--max-relative-linf",
        type=float,
        default=None,
        help="Fail when the relative L-infinity velocity-profile error exceeds this value.",
    )
    parser.add_argument(
        "--max-relative-mass-drift",
        type=float,
        default=None,
        help="Fail when the relative fluid-mass drift exceeds this value.",
    )
    parser.add_argument(
        "--max-density-deviation",
        type=float,
        default=None,
        help="Fail when max(abs(rho - 1)) exceeds this value.",
    )
    parser.add_argument(
        "--max-transverse-velocity-ratio",
        type=float,
        default=None,
        help="Fail when max(abs(uy)) divided by the analytical peak ux exceeds this value.",
    )
    parser.add_argument(
        "--max-final-change",
        type=float,
        default=None,
        help="Fail when the final recorded maximum ux change exceeds this value.",
    )
    return parser.parse_args()


def require_file(path: Path) -> None:
    if not path.exists():
        raise FileNotFoundError(
            f"Missing {path}. Run the solver first, for example: ./lbm_channel"
        )


def require_columns(frame: pd.DataFrame, required: set[str], source: Path) -> None:
    missing = sorted(required.difference(frame.columns))
    if missing:
        raise ValueError(f"{source} is missing required columns: {', '.join(missing)}")


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


def verification_metrics(
    profile: pd.DataFrame,
    convergence: pd.DataFrame,
    velocity: pd.DataFrame,
) -> dict[str, float]:
    profile_error = (profile["lbm_ux"] - profile["analytical_ux"]).abs()
    max_absolute_error = float(profile_error.max())
    reference_velocity = float(profile["analytical_ux"].abs().max())
    relative_linf = (
        max_absolute_error / reference_velocity
        if reference_velocity > 0.0
        else float("nan")
    )

    expected_mass = float(len(velocity))
    final_mass = float(velocity["rho"].sum())
    relative_mass_drift = (
        abs(final_mass - expected_mass) / expected_mass
        if expected_mass > 0.0
        else float("nan")
    )

    max_density_deviation = float((velocity["rho"] - 1.0).abs().max())
    max_abs_uy = float(velocity["uy"].abs().max())
    transverse_velocity_ratio = (
        max_abs_uy / reference_velocity
        if reference_velocity > 0.0
        else float("nan")
    )
    final_max_change = float(convergence["max_change"].iloc[-1])

    return {
        "max_absolute_profile_error": max_absolute_error,
        "relative_linf_profile_error": relative_linf,
        "relative_mass_drift": relative_mass_drift,
        "max_density_deviation": max_density_deviation,
        "max_abs_transverse_velocity": max_abs_uy,
        "transverse_velocity_ratio": transverse_velocity_ratio,
        "final_max_ux_change": final_max_change,
    }


def print_verification_summary(metrics: dict[str, float]) -> None:
    print("Verification summary")
    for name, value in metrics.items():
        print(f"  {name:32s}: {value:.6e}")


def enforce_upper_limit(name: str, value: float, limit: float | None) -> None:
    if limit is None:
        return
    if limit <= 0.0:
        raise ValueError(f"The limit for {name} must be greater than zero.")
    if not math.isfinite(value):
        raise SystemExit(f"Verification failed: {name} is not finite.")
    if value > limit:
        raise SystemExit(
            f"Verification failed: {name}={value:.6e} exceeds {limit:.6e}."
        )
    print(f"Verification passed: {name} <= {limit:.6e}")


def save_summary(
    metrics: dict[str, float],
    limits: dict[str, float | None],
) -> None:
    rows = []
    for name, value in metrics.items():
        limit = limits.get(name)
        passed = math.isfinite(value) and (limit is None or value <= limit)
        rows.append(
            {
                "metric": name,
                "value": value,
                "limit": limit,
                "status": "pass" if passed else "fail",
            }
        )
    pd.DataFrame(rows).to_csv(SUMMARY_FILE, index=False)


def main() -> None:
    args = parse_args()
    for path in (PROFILE_FILE, CONVERGENCE_FILE, VELOCITY_FILE):
        require_file(path)

    profile = pd.read_csv(PROFILE_FILE)
    convergence = pd.read_csv(CONVERGENCE_FILE)
    velocity = pd.read_csv(VELOCITY_FILE)

    require_columns(profile, {"y", "lbm_ux", "analytical_ux"}, PROFILE_FILE)
    require_columns(convergence, {"step", "max_change"}, CONVERGENCE_FILE)
    require_columns(velocity, {"x", "y", "rho", "ux", "uy", "speed"}, VELOCITY_FILE)

    plot_velocity_profile(profile)
    plot_convergence(convergence)

    metrics = verification_metrics(profile, convergence, velocity)
    limits = {
        "relative_linf_profile_error": args.max_relative_linf,
        "relative_mass_drift": args.max_relative_mass_drift,
        "max_density_deviation": args.max_density_deviation,
        "transverse_velocity_ratio": args.max_transverse_velocity_ratio,
        "final_max_ux_change": args.max_final_change,
    }

    print_verification_summary(metrics)
    save_summary(metrics, limits)

    for name, limit in limits.items():
        enforce_upper_limit(name, metrics[name], limit)

    print(f"Saved plots and verification summary in {RESULTS_DIR}/")


if __name__ == "__main__":
    main()
