# D2Q9 Lattice Boltzmann Solver for Poiseuille Flow

<p align="center">
  <img src="https://img.shields.io/badge/C++-17-blue.svg" alt="C++17">
  <img src="https://img.shields.io/badge/Method-LBM%20D2Q9-green.svg" alt="LBM D2Q9">
  <img src="https://img.shields.io/badge/Validation-Poiseuille%20Flow-yellow.svg" alt="Poiseuille flow validation">
  <a href="https://github.com/Kandil2001/LBM_Poiseuille_D2Q9_CPP/actions/workflows/ci.yml">
    <img src="https://github.com/Kandil2001/LBM_Poiseuille_D2Q9_CPP/actions/workflows/ci.yml/badge.svg" alt="Build and run workflow">
  </a>
  <a href="LICENSE">
    <img src="https://img.shields.io/badge/License-MIT-lightgrey.svg" alt="MIT License">
  </a>
  <a href="https://kandil2001.github.io/">
    <img src="https://img.shields.io/badge/Portfolio-kandil2001.github.io-2ea44f.svg" alt="Portfolio">
  </a>
</p>

A compact C++ implementation of the **Lattice Boltzmann Method (LBM)** for two-dimensional pressure/body-force-driven Poiseuille flow in a channel.

The goal of this repository is not to be a large CFD framework. It is a clean validation case that shows the main LBM steps clearly: D2Q9 lattice setup, BGK collision, streaming, bounce-back wall boundaries, periodic streamwise direction, and comparison against the analytical parabolic velocity profile.

<p align="center">
  <img src="results/velocity_profile.png" width="720" alt="LBM velocity profile compared with analytical Poiseuille solution">
</p>

## What this project demonstrates

- D2Q9 lattice Boltzmann implementation in modern C++
- BGK single-relaxation-time collision model
- Guo-style body-force term for driving the channel flow
- No-slip upper and lower walls using bounce-back treatment
- Periodic boundary condition in the streamwise direction
- CSV output for velocity fields, centerline validation, and convergence history
- Python post-processing for validation and convergence figures
- Basic CI workflow for building, running, and plotting a short validation case
- Simple structure for future extension to grid studies, OpenMP, MPI, or more complex boundary conditions

## Physical case

The simulation represents laminar flow between two parallel plates. A small constant force is applied in the streamwise direction, and the final velocity profile is compared with the analytical Poiseuille solution.

The numerical loop is:

```text
compute macroscopic fields -> collide -> stream -> apply wall bounce-back -> repeat
```

The current implementation keeps the setup intentionally small so the numerical method remains easy to inspect and modify.

## Repository structure

```text
.
├── .github/
│   ├── ISSUE_TEMPLATE/          # bug report and feature request templates
│   ├── workflows/ci.yml         # build-and-run GitHub Actions workflow
│   ├── dependabot.yml           # monthly dependency checks
│   └── PULL_REQUEST_TEMPLATE.md # pull request checklist
├── include/
│   └── lbm.hpp                  # D2Q9 constants, parameters, and helper functions
├── src/
│   └── main.cpp                 # solver implementation and CSV output
├── scripts/
│   └── plot_results.py          # post-processing and validation plots
├── results/
│   ├── centerline_profile.csv   # numerical vs analytical centerline profile
│   ├── convergence.csv          # convergence history
│   ├── run_info.txt             # parameters used in the run
│   ├── velocity_field.csv       # full velocity field output
│   ├── velocity_profile.png     # validation plot
│   └── convergence.png          # convergence plot
├── CODE_OF_CONDUCT.md           # community behavior expectations
├── CONTRIBUTING.md              # contribution workflow and checklist
├── CITATION.cff                 # citation metadata
├── LICENSE                      # MIT license
├── SECURITY.md                  # vulnerability reporting policy
├── Makefile                     # build, run, plot, and clean targets
├── requirements.txt             # Python plotting dependencies
└── README.md
```

## Build

Requirements:

- A C++17 compiler such as `g++`
- `make`
- Python 3 with `numpy`, `pandas`, and `matplotlib` for plotting

Build the solver:

```bash
git clone https://github.com/Kandil2001/LBM_Poiseuille_D2Q9_CPP.git
cd LBM_Poiseuille_D2Q9_CPP
make
```

## Run

Run the default case:

```bash
./lbm_channel
```

Run a custom case:

```bash
./lbm_channel nx ny steps tau forceX [saveEvery]
```

Example used for the included validation figures:

```bash
./lbm_channel 160 50 20000 0.8 1e-6 200
```

The parameters are:

| Parameter | Meaning | Default |
|---|---|---:|
| `nx` | number of lattice nodes in the streamwise direction | `160` |
| `ny` | number of lattice nodes in the wall-normal direction | `50` |
| `steps` | number of time steps | `20000` |
| `tau` | relaxation time; must be greater than `0.5` | `0.8` |
| `forceX` | constant body force in the streamwise direction | `1e-6` |
| `saveEvery` | interval for writing convergence values | `200` |

## Plot the results

Install the plotting dependencies:

```bash
python3 -m pip install -r requirements.txt
```

Generate the validation and convergence figures:

```bash
make plots
```

or directly:

```bash
python3 scripts/plot_results.py
```

## Example output

The default run writes the following files to `results/`:

| File | Description |
|---|---|
| `centerline_profile.csv` | numerical velocity profile and analytical Poiseuille reference |
| `velocity_field.csv` | full velocity field with `ux`, `uy`, and speed |
| `convergence.csv` | maximum change in `ux` over the saved intervals |
| `run_info.txt` | run parameters and derived viscosity |
| `velocity_profile.png` | comparison between LBM and analytical velocity profile |
| `convergence.png` | convergence history |

<p align="center">
  <img src="results/convergence.png" width="680" alt="LBM convergence history">
</p>

## Project quality files

This repository includes common project-maintenance files:

- `LICENSE` for reuse terms
- `SECURITY.md` for responsible reporting of security-related concerns
- `CONTRIBUTING.md` for contribution workflow and validation expectations
- `CODE_OF_CONDUCT.md` for respectful project communication
- `CITATION.cff` so GitHub can show a citation button
- issue and pull request templates for cleaner collaboration
- Dependabot configuration for monthly dependency update checks

## Notes and limitations

This is an educational CFD validation project. The code is written to be readable and easy to extend, not to compete with optimized production CFD solvers.

Current limitations:

- single relaxation time BGK model only
- one simple channel-flow validation case
- serial implementation
- no grid-convergence automation yet
- basic bounce-back boundary handling

Useful next steps include a grid convergence study, Reynolds number variation, OpenMP parallelization, MPI domain decomposition, and comparison against finite-difference or finite-volume solvers.

## Citation

If you use this repository for learning, teaching, or research, please cite it using the metadata in `CITATION.cff`.

## License

This project is released under the MIT License. See `LICENSE` for details.

## Author

Ahmed Kandil  
M.Sc. Computer Simulation in Science, Bergische Universität Wuppertal  
[Portfolio](https://kandil2001.github.io/) · [GitHub](https://github.com/Kandil2001) · [LinkedIn](https://www.linkedin.com/in/ahmed-kandil03/)
