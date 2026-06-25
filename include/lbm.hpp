#pragma once

#include <array>

struct Params {
    int nx = 160;          // lattice nodes in the streamwise direction
    int ny = 50;           // lattice nodes in the wall-normal direction
    int steps = 20000;     // total time steps
    int saveEvery = 200;   // interval for convergence output

    double tau = 0.8;          // relaxation time; stable values require tau > 0.5
    double forceX = 1.0e-6;    // streamwise body force
};

// D2Q9 velocity directions:
// 6 2 5
// 3 0 1
// 7 4 8
inline constexpr std::array<int, 9> cx  = {0, 1, 0, -1, 0, 1, -1, -1, 1};
inline constexpr std::array<int, 9> cy  = {0, 0, 1, 0, -1, 1, 1, -1, -1};
inline constexpr std::array<int, 9> opp = {0, 3, 4, 1, 2, 7, 8, 5, 6};

inline constexpr std::array<double, 9> w = {
    4.0 / 9.0,
    1.0 / 9.0, 1.0 / 9.0, 1.0 / 9.0, 1.0 / 9.0,
    1.0 / 36.0, 1.0 / 36.0, 1.0 / 36.0, 1.0 / 36.0
};

[[nodiscard]] inline int id(int x, int y, int nx) {
    return y * nx + x;
}

[[nodiscard]] inline double viscosity(double tau) {
    return (tau - 0.5) / 3.0;
}

[[nodiscard]] inline double equilibrium(int k, double rho, double ux, double uy) {
    const double cu = cx[k] * ux + cy[k] * uy;
    const double u2 = ux * ux + uy * uy;

    return w[k] * rho * (1.0 + 3.0 * cu + 4.5 * cu * cu - 1.5 * u2);
}
