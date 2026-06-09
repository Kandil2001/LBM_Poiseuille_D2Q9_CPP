#include <algorithm>
#include <array>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>

#include "lbm.hpp"

using Cell = std::array<double, 9>;

void readCommandLine(int argc, char** argv, Params& p) {
    if (argc > 1) p.nx = std::stoi(argv[1]);
    if (argc > 2) p.ny = std::stoi(argv[2]);
    if (argc > 3) p.steps = std::stoi(argv[3]);
    if (argc > 4) p.tau = std::stod(argv[4]);
    if (argc > 5) p.forceX = std::stod(argv[5]);
}

void calculateRhoAndVelocity(
    const std::vector<Cell>& f,
    std::vector<double>& rho,
    std::vector<double>& ux,
    std::vector<double>& uy,
    const Params& p
) {
    for (int y = 1; y < p.ny - 1; ++y) {
        for (int x = 0; x < p.nx; ++x) {
            int n = id(x, y, p.nx);

            double r = 0.0;
            double mx = 0.0;
            double my = 0.0;

            for (int k = 0; k < 9; ++k) {
                r  += f[n][k];
                mx += f[n][k] * cx[k];
                my += f[n][k] * cy[k];
            }

            rho[n] = r;

            // The 0.5 * force part is the usual correction for forced LBM.
            ux[n] = (mx + 0.5 * p.forceX) / r;
            uy[n] = my / r;
        }
    }
}

void collide(
    const std::vector<Cell>& f,
    std::vector<Cell>& afterCollision,
    const std::vector<double>& rho,
    const std::vector<double>& ux,
    const std::vector<double>& uy,
    const Params& p
) {
    double omega = 1.0 / p.tau;

    for (int y = 1; y < p.ny - 1; ++y) {
        for (int x = 0; x < p.nx; ++x) {
            int n = id(x, y, p.nx);

            for (int k = 0; k < 9; ++k) {
                double feq = equilibrium(k, rho[n], ux[n], uy[n]);

                // Guo force term. It looks a bit ugly, but it only adds the body force.
                // Here force is only in x, so Fy = 0.
                double cu = cx[k] * ux[n] + cy[k] * uy[n];
                double forceTerm = w[k] * (1.0 - 0.5 * omega)
                    * (3.0 * (cx[k] - ux[n]) + 9.0 * cu * cx[k])
                    * p.forceX;

                afterCollision[n][k] = f[n][k] - omega * (f[n][k] - feq) + forceTerm;
            }
        }
    }
}

void streamAndBounce(
    const std::vector<Cell>& afterCollision,
    std::vector<Cell>& fNew,
    const Params& p
) {
    for (auto& cell : fNew) {
        cell.fill(0.0);
    }

    for (int y = 1; y < p.ny - 1; ++y) {
        for (int x = 0; x < p.nx; ++x) {
            int n = id(x, y, p.nx);

            for (int k = 0; k < 9; ++k) {
                int x2 = (x + cx[k] + p.nx) % p.nx; // periodic left/right
                int y2 = y + cy[k];

                // top and bottom walls: simple bounce-back
                if (y2 == 0 || y2 == p.ny - 1) {
                    fNew[n][opp[k]] += afterCollision[n][k];
                } else {
                    fNew[id(x2, y2, p.nx)][k] += afterCollision[n][k];
                }
            }
        }
    }
}

void saveVelocityField(
    const std::vector<double>& ux,
    const std::vector<double>& uy,
    const Params& p
) {
    std::ofstream file("results/velocity_field.csv");
    file << "x,y,ux,uy,speed\n";

    for (int y = 1; y < p.ny - 1; ++y) {
        for (int x = 0; x < p.nx; ++x) {
            int n = id(x, y, p.nx);
            double speed = std::sqrt(ux[n] * ux[n] + uy[n] * uy[n]);
            file << x << ',' << y << ',' << ux[n] << ',' << uy[n] << ',' << speed << '\n';
        }
    }
}

void saveCenterProfile(
    const std::vector<double>& ux,
    const Params& p
) {
    std::ofstream file("results/centerline_profile.csv");
    file << "y,lbm_ux,analytical_ux\n";

    int x = p.nx / 2;
    double nu = viscosity(p.tau);

    // Effective channel height for halfway bounce-back walls.
    double H = static_cast<double>(p.ny - 2);

    for (int y = 1; y < p.ny - 1; ++y) {
        int n = id(x, y, p.nx);

        // y position measured from the bottom wall.
        double yy = y - 0.5;
        double analytical = p.forceX * yy * (H - yy) / (2.0 * nu);

        file << y << ',' << ux[n] << ',' << analytical << '\n';
    }
}

void saveRunInfo(const Params& p) {
    std::ofstream file("results/run_info.txt");
    file << "LBM Poiseuille flow, D2Q9, BGK\n";
    file << "nx = " << p.nx << '\n';
    file << "ny = " << p.ny << '\n';
    file << "steps = " << p.steps << '\n';
    file << "tau = " << p.tau << '\n';
    file << "viscosity = " << viscosity(p.tau) << '\n';
    file << "forceX = " << p.forceX << '\n';
}

int main(int argc, char** argv) {
    Params p;
    readCommandLine(argc, argv, p);

    if (p.ny < 5 || p.nx < 5 || p.tau <= 0.5) {
        std::cerr << "Use nx >= 5, ny >= 5 and tau > 0.5\n";
        return 1;
    }

    std::filesystem::create_directories("results");

    int totalCells = p.nx * p.ny;

    std::vector<Cell> f(totalCells);
    std::vector<Cell> afterCollision(totalCells);
    std::vector<Cell> fNew(totalCells);

    std::vector<double> rho(totalCells, 1.0);
    std::vector<double> ux(totalCells, 0.0);
    std::vector<double> uy(totalCells, 0.0);
    std::vector<double> oldUx(totalCells, 0.0);

    // Start from fluid at rest.
    for (int y = 0; y < p.ny; ++y) {
        for (int x = 0; x < p.nx; ++x) {
            int n = id(x, y, p.nx);
            for (int k = 0; k < 9; ++k) {
                f[n][k] = equilibrium(k, 1.0, 0.0, 0.0);
            }
        }
    }

    std::ofstream convergence("results/convergence.csv");
    convergence << "step,max_change\n";

    std::cout << "Running simple LBM channel flow...\n";

    for (int step = 1; step <= p.steps; ++step) {
        calculateRhoAndVelocity(f, rho, ux, uy, p);
        collide(f, afterCollision, rho, ux, uy, p);
        streamAndBounce(afterCollision, fNew, p);
        f.swap(fNew);

        if (step % p.saveEvery == 0 || step == 1) {
            calculateRhoAndVelocity(f, rho, ux, uy, p);

            double maxChange = 0.0;
            for (int y = 1; y < p.ny - 1; ++y) {
                for (int x = 0; x < p.nx; ++x) {
                    int n = id(x, y, p.nx);
                    maxChange = std::max(maxChange, std::abs(ux[n] - oldUx[n]));
                    oldUx[n] = ux[n];
                }
            }

            convergence << step << ',' << maxChange << '\n';
            std::cout << "step " << step << "   max ux change = " << maxChange << '\n';
        }
    }

    calculateRhoAndVelocity(f, rho, ux, uy, p);
    saveVelocityField(ux, uy, p);
    saveCenterProfile(ux, p);
    saveRunInfo(p);

    std::cout << "Done. Results are in the results folder.\n";
    return 0;
}
