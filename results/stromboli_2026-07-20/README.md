# Stromboli validation — 20 July 2026

This directory archives the LBM Poiseuille validation run executed on the Stromboli HPC cluster.

## Configuration

- compiler on the compute node: GCC 8.5.0
- lattice size: `80 × 30`
- time steps: `12,000`
- relaxation time: `tau = 0.8`
- body force: `forceX = 1e-6`
- convergence-output interval: `200`

## Validation result

The sampled numerical velocity profile differs from the analytical Poiseuille profile by a maximum absolute value of `6.50e-7`.

Using the maximum analytical velocity as the reference, the relative `L_inf` error is:

```text
6.641e-4 = 0.0664%
```

The repository regression threshold is `0.05` (`5%`), so this run passed the analytical validation gate.

## Main files

- `run_info.txt` — solver configuration
- `centerline_profile.csv` — numerical and analytical centerline velocities
- `convergence.csv` — saved convergence history
- `velocity_field.csv` — full velocity field, when retained in the archive
- generated PNG files — profile and convergence visualizations, when retained

The CSV files are the canonical numerical record.
