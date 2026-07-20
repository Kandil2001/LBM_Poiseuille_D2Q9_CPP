# D2Q9 Lattice Boltzmann Solver for Poiseuille Flow

<p align="center">
  <img src="https://img.shields.io/badge/Status-Completed-brightgreen.svg" alt="Completed">
  <img src="https://img.shields.io/badge/C%2B%2B-17-blue.svg" alt="C++17">
  <img src="https://img.shields.io/badge/Method-LBM%20D2Q9-green.svg" alt="LBM D2Q9">
  <img src="https://img.shields.io/badge/Validation-Analytical%20Poiseuille-brightgreen.svg" alt="Analytical Poiseuille validation">
  <a href="https://github.com/Kandil2001/LBM_Poiseuille_D2Q9_CPP/actions/workflows/ci.yml">
    <img src="https://github.com/Kandil2001/LBM_Poiseuille_D2Q9_CPP/actions/workflows/ci.yml/badge.svg" alt="Build, run, and validate workflow">
  </a>
  <a href="LICENSE">
    <img src="https://img.shields.io/badge/License-MIT-lightgrey.svg" alt="MIT License">
  </a>
  <a href="https://kandil2001.github.io/projects/lbm-poiseuille.html">
    <img src="https://img.shields.io/badge/Portfolio-Case%20Study-2ea44f.svg" alt="Portfolio case study">
  </a>
</p>

A compact C++ implementation of the **Lattice Boltzmann Method (LBM)** for two-dimensional body-force-driven Poiseuille flow in a channel.

The project exposes the main LBM steps clearly: D2Q9 lattice setup, BGK collision, streaming, bounce-back wall boundaries, periodic streamwise treatment, and comparison with the analytical parabolic velocity profile.

<p align="center">
  <img src="results/velocity_profile.png" width="720" alt="LBM velocity profile compared with analytical Poiseuille solution">
</p>

## What this project demonstrates

- D2Q9 Lattice Boltzmann implementation in modern C++
- BGK single-relaxation-time collision model
- Guo-style body-force term
- no-slip upper and lower walls using bounce-back treatment
- periodic boundary condition in the streamwise direction
- CSV output for velocity fields, centerline validation, and convergence history
- Python post-processing for validation and convergence figures
- CI workflow that builds, runs, plots, verifies outputs, and enforces an analytical error limit
- compatibility with current compilers and older GCC 8 cluster toolchains

## Physical case

The simulation represents laminar flow between two parallel plates. A small constant force drives the flow in the streamwise direction, and the final velocity profile is compared with the analytical Poiseuille solution.

```text
compute macroscopic fields → collide → stream → apply wall bounce-back → repeat
```

The implementation remains intentionally compact so the numerical method is easy to inspect and modify.

## Build

Requirements:

- a C++17 compiler such as `g++`
- `make`
- Python 3 with `pandas` and `matplotlib` for plotting

```bash
git clone https://github.com/Kandil2001/LBM_Poiseuille_D2Q9_CPP.git
cd LBM_Poiseuille_D2Q9_CPP
make
```

The Makefile first uses the normal C++17 link command. If an older GCC toolchain still requires the separate filesystem library, it automatically retries with `-lstdc++fs`. This keeps the same build command working on modern systems and on GCC 8-based HPC nodes.

## Run

Run the default case:

```bash
./lbm_channel
```

Run a custom case:

```bash
./lbm_channel nx ny steps tau forceX [saveEvery]
```

Example used for the main validation figures:

```bash
./lbm_channel 160 50 20000 0.8 1e-6 200
```

| Parameter | Meaning | Default |
|---|---|---:|
| `nx` | lattice nodes in the streamwise direction | `160` |
| `ny` | lattice nodes in the wall-normal direction | `50` |
| `steps` | number of time steps | `20000` |
| `tau` | relaxation time; must be greater than `0.5` | `0.8` |
| `forceX` | constant streamwise body force | `1e-6` |
| `saveEvery` | convergence-output interval | `200` |

## Plot and validate the results

Install the plotting dependencies:

```bash
python3 -m pip install -r requirements.txt
```

Generate the figures:

```bash
make plots
```

Enforce the repository's analytical regression threshold:

```bash
make validate
```

The validation command exits with a nonzero status when the relative `L_inf` profile error is non-finite or exceeds `0.05`.

## Stromboli validation — 20 July 2026

A fresh validation case was built and run on the Stromboli HPC cluster with GCC 8.5.0.

| Parameter | Value |
|---|---:|
| Grid | `80 × 30` |
| Time steps | `12,000` |
| Relaxation time `tau` | `0.8` |
| Body force `forceX` | `1e-6` |
| Output interval | `200` |
| Maximum absolute profile error | `6.50e-7` |
| Relative `L_inf` profile error | `6.641e-4` (`0.0664%`) |
| Validation threshold | `0.05` (`5%`) |
| Result | **Passed** |

The numerical centerline velocity differs from the analytical profile by approximately `6.5e-7` at the sampled points. The complete archived output is in [`results/stromboli_2026-07-20`](results/stromboli_2026-07-20).

## Continuous integration

The GitHub Actions workflow:

1. builds the C++17 solver
2. runs an `80 × 30`, `12,000`-step Poiseuille case
3. generates validation and convergence figures
4. requires a relative `L_inf` velocity-profile error of at most `0.05`
5. verifies all expected CSV, text, and PNG outputs

The threshold is a regression gate for this controlled analytical case. It is not a substitute for a formal grid-convergence or uncertainty study.

## Generated output

| File | Description |
|---|---|
| `centerline_profile.csv` | numerical velocity profile and analytical Poiseuille reference |
| `velocity_field.csv` | full velocity field with `ux`, `uy`, and speed |
| `convergence.csv` | maximum change in `ux` over saved intervals |
| `run_info.txt` | run parameters and derived viscosity |
| `velocity_profile.png` | numerical and analytical profile comparison |
| `convergence.png` | convergence history |

<p align="center">
  <img src="results/convergence.png" width="680" alt="LBM convergence history">
</p>

## Repository structure

```text
include/lbm.hpp                         D2Q9 constants and helper functions
src/main.cpp                            solver and CSV output
scripts/plot_results.py                 plotting and analytical validation
results/                                reference output used in the README
results/stromboli_2026-07-20/           archived HPC validation run
.github/workflows/ci.yml                automated build and validation
Makefile                                build, run, plot, validate, and clean targets
requirements.txt                        Python plotting dependencies
```

## Scope and limitations

This is an educational CFD validation case designed for readability and extension rather than production performance.

Current scope:

- single-relaxation-time BGK model
- one channel-flow validation case
- serial implementation
- basic bounce-back boundary treatment
- no automated grid-convergence study

Possible follow-up work includes grid refinement, Reynolds-number variation, MRT collision, OpenMP or MPI parallelization, and comparison with finite-difference or finite-volume solvers.

## Citation

Use the metadata in [`CITATION.cff`](CITATION.cff) when citing this project.

## Author

Ahmed Kandil — [Portfolio](https://kandil2001.github.io/) · [LinkedIn](https://www.linkedin.com/in/ahmed-kandil03/) · [ORCID](https://orcid.org/0009-0007-2724-4565)

Released under the [MIT License](LICENSE).
