# Simple LBM Channel Flow in C++

This is a small first LBM code.

It solves **2D Poiseuille flow** using the **D2Q9 lattice Boltzmann method**.

The idea is simple:

```text
collide -> stream -> bounce at walls -> repeat
```

The flow is pushed by a small force in the x direction.
The top and bottom are walls.
The left and right sides are periodic, so fluid leaving one side enters from the other side.

## Why this project

I made this as the easiest CFD case for learning LBM.

It is useful because the final velocity profile should become a parabola, so the code can be checked against the analytical solution.

## Files

```text
include/lbm.hpp      D2Q9 constants and small helper functions
src/main.cpp         the full solver
scripts/plot_results.py   simple plotting script
results/             output folder
Makefile             build file
```

I kept the code divided, but not too much. There are no classes and no complicated structure.

## Build

```bash
make
```

## Run

```bash
./lbm_channel
```

Or run with your own values:

```bash
./lbm_channel nx ny steps tau forceX
```

Example:

```bash
./lbm_channel 160 50 20000 0.8 1e-6
```

## Output

After running, the code writes:

```text
results/centerline_profile.csv
results/velocity_field.csv
results/convergence.csv
results/run_info.txt
```

The most important one is:

```text
results/centerline_profile.csv
```

It has the LBM velocity and the analytical velocity profile.

## Plot

```bash
python scripts/plot_results.py
```

This creates:

```text
results/velocity_profile.png
results/convergence.png
```

## What to look for

The LBM velocity profile should look close to a parabola.

If the velocity is too small, increase `forceX` a little.
If the simulation becomes unstable, reduce `forceX` or increase `tau`.

Good first values:

```text
nx = 160
ny = 50
steps = 20000
tau = 0.8
forceX = 1e-6
```
