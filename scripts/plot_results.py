from pathlib import Path

import matplotlib.pyplot as plt
import pandas as pd


RESULTS_DIR = Path("results")
PROFILE_FILE = RESULTS_DIR / "centerline_profile.csv"
CONVERGENCE_FILE = RESULTS_DIR / "convergence.csv"


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


def plot_convergence(convergence: pd.DataFrame) -> None:
    plt.figure()
    plt.semilogy(convergence["step"], convergence["max_change"])
    plt.xlabel("Time step")
    plt.ylabel("Maximum change in ux")
    plt.title("Convergence history")
    plt.grid(True)
    plt.tight_layout()
    plt.savefig(RESULTS_DIR / "convergence.png", dpi=200)


def print_validation_summary(profile: pd.DataFrame) -> None:
    error = (profile["lbm_ux"] - profile["analytical_ux"]).abs()
    reference = profile["analytical_ux"].abs().max()
    relative_linf = error.max() / reference if reference > 0.0 else float("nan")

    print("Validation summary")
    print(f"  max absolute error  : {error.max():.6e}")
    print(f"  relative L_inf error: {relative_linf:.6e}")


def main() -> None:
    require_file(PROFILE_FILE)
    require_file(CONVERGENCE_FILE)

    profile = pd.read_csv(PROFILE_FILE)
    convergence = pd.read_csv(CONVERGENCE_FILE)

    plot_velocity_profile(profile)
    plot_convergence(convergence)
    print_validation_summary(profile)
    print(f"Saved plots in {RESULTS_DIR}/")


if __name__ == "__main__":
    main()
