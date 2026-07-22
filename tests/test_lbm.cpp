#include <array>
#include <cmath>
#include <cstdlib>
#include <iostream>

#include "lbm.hpp"

namespace {

bool close(double actual, double expected, double tolerance = 1.0e-12) {
    return std::abs(actual - expected) <= tolerance;
}

void require(bool condition, const char* message) {
    if (!condition) {
        std::cerr << "Test failed: " << message << '\n';
        std::exit(EXIT_FAILURE);
    }
}

void testWeightsAndOpposites() {
    double weightSum = 0.0;
    for (int k = 0; k < 9; ++k) {
        weightSum += w[k];
        require(opp[opp[k]] == k, "opposite-direction mapping must be symmetric");
        require(cx[opp[k]] == -cx[k], "opposite x direction is inconsistent");
        require(cy[opp[k]] == -cy[k], "opposite y direction is inconsistent");
    }
    require(close(weightSum, 1.0), "D2Q9 weights must sum to one");
}

void testEquilibriumMomentsAtRest() {
    constexpr double rho = 1.23;
    double recoveredRho = 0.0;
    double recoveredMx = 0.0;
    double recoveredMy = 0.0;

    for (int k = 0; k < 9; ++k) {
        const double feq = equilibrium(k, rho, 0.0, 0.0);
        recoveredRho += feq;
        recoveredMx += feq * cx[k];
        recoveredMy += feq * cy[k];
    }

    require(close(recoveredRho, rho), "equilibrium must recover density at rest");
    require(close(recoveredMx, 0.0), "equilibrium x momentum must vanish at rest");
    require(close(recoveredMy, 0.0), "equilibrium y momentum must vanish at rest");
}

void testEquilibriumMomentsWithVelocity() {
    constexpr double rho = 0.97;
    constexpr double ux = 0.04;
    constexpr double uy = -0.02;
    double recoveredRho = 0.0;
    double recoveredMx = 0.0;
    double recoveredMy = 0.0;

    for (int k = 0; k < 9; ++k) {
        const double feq = equilibrium(k, rho, ux, uy);
        recoveredRho += feq;
        recoveredMx += feq * cx[k];
        recoveredMy += feq * cy[k];
    }

    require(close(recoveredRho, rho), "equilibrium must recover density");
    require(close(recoveredMx, rho * ux), "equilibrium must recover x momentum");
    require(close(recoveredMy, rho * uy), "equilibrium must recover y momentum");
}

void testHelpers() {
    require(id(3, 2, 10) == 23, "linear index helper is incorrect");
    require(close(viscosity(0.8), 0.1), "viscosity relation is incorrect");
    require(close(viscosity(0.5), 0.0), "viscosity at tau=0.5 must be zero");
}

}  // namespace

int main() {
    testWeightsAndOpposites();
    testEquilibriumMomentsAtRest();
    testEquilibriumMomentsWithVelocity();
    testHelpers();
    std::cout << "All LBM core tests passed.\n";
    return EXIT_SUCCESS;
}
