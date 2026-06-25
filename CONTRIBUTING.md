# Contributing

Thank you for your interest in improving this LBM Poiseuille flow solver.

This repository is mainly an educational and validation-focused CFD project. Contributions should keep the code readable, reproducible, and easy to compare against the analytical Poiseuille solution.

## Good contribution ideas

- improve documentation or comments
- add validation cases or grid-convergence studies
- improve plotting and error metrics
- add OpenMP or MPI versions while keeping the serial solver understandable
- improve build instructions for Linux, Windows, or HPC systems
- fix numerical or boundary-condition issues

## Development workflow

1. Fork the repository or create a feature branch.
2. Make a focused change.
3. Build the solver:

```bash
make
```

4. Run a short validation case:

```bash
./lbm_channel 80 30 1000 0.8 1e-6 100
```

5. Generate plots:

```bash
python3 -m pip install -r requirements.txt
make plots
```

6. Open a pull request and explain what changed and why.

## Code style

- Prefer clear C++17 over overly clever code.
- Keep numerical parameters explicit.
- Use descriptive names for physical quantities.
- Avoid adding large generated files unless they are needed for the README or validation.
- Keep the default case small enough to run quickly on a normal laptop.

## Pull request checklist

Before opening a pull request, please check:

- [ ] The code builds with `make`.
- [ ] A short validation case runs successfully.
- [ ] Plot generation works, if plotting was affected.
- [ ] The README or documentation was updated if user-facing behavior changed.
- [ ] Generated executables, temporary files, and large result dumps are not committed.

## Reporting issues

For bugs or numerical problems, please include:

- operating system and compiler
- exact command used to run the solver
- parameter values
- expected behavior
- actual behavior
- relevant output or plots
