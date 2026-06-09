#ifndef LBM_HPP
#define LBM_HPP

#include <array>

// Small file for the lattice constants and the input values.
// I kept this separate so main.cpp is easier to read.

struct Params {
    int nx = 160;              // cells in x direction
    int ny = 50;               // cells in y direction
    int steps = 20000;         // time steps
    int saveEvery = 200;       // how often convergence is saved

    double tau = 0.8;          // relaxation time, must be > 0.5
    double forceX = 1.0e-6;    // small force pushing the flow to the right
};

// D2Q9 directions
// 6 2 5
// 3 0 1
// 7 4 8
static const std::array<int, 9> cx  = {0, 1, 0,-1, 0, 1,-1,-1, 1};
static const std::array<int, 9> cy  = {0, 0, 1, 0,-1, 1, 1,-1,-1};
static const std::array<int, 9> opp = {0, 3, 4, 1, 2, 7, 8, 5, 6};

static const std::array<double, 9> w = {
    4.0/9.0,
    1.0/9.0, 1.0/9.0, 1.0/9.0, 1.0/9.0,
    1.0/36.0, 1.0/36.0, 1.0/36.0, 1.0/36.0
};

inline int id(int x, int y, int nx) {
    return y * nx + x;
}

inline double viscosity(double tau) {
    return (tau - 0.5) / 3.0;
}

inline double equilibrium(int k, double rho, double ux, double uy) {
    double cu = cx[k] * ux + cy[k] * uy;
    double u2 = ux * ux + uy * uy;

    return w[k] * rho * (1.0 + 3.0 * cu + 4.5 * cu * cu - 1.5 * u2);
}

#endif
