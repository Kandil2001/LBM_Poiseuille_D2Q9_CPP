#include <algorithm>
#include <array>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "lbm.hpp"

using Cell = std::array<double, 9>;

void printUsage(const char* executableName) {
    std::cout
        << "Usage:\n"
        << "  " << executableName << "\n"
        << "  " << executableName << " nx ny steps tau forceX [saveEvery]\n\n"
        << "Example:\n"
        << "  " << executableName << " 160 50 20000 0.8 1e-6 200\n\n"
        << "Constraints:\n"
        << "  nx >= 5, ny >= 5, steps > 0, tau > 0.5, saveEvery > 0\n";
}

bool readCommandLine(int argc, char** argv, Params& p) {
    if (argc > 1) {
        const std::string firstArg = argv[1];
        if (firstArg == "-h" || firstArg == "--help") {
            printUsage(argv[0]);
            return false;
        }
    }

    if (argc != 1 && argc != 6 && argc != 7) {
        printUsage(argv[0]);
        throw std::invalid_argument("Invalid number of command-line arguments.");
    }

    if (argc >= 6) {
        p.nx = std::stoi(argv[1]);
        p.ny = std::stoi(argv[2]);
        p.steps = std::stoi(argv[3]);
        p.tau = std::stod(argv[4]);
        p.forceX = std::stod(argv[5]);
    }

    if (argc == 7) {
        p.saveEvery = std::stoi(argv[6]);
    }

    return true;
}

void validateParameters(const Params& p) {
    if (p.nx < 5 || p.ny < 5) {
        throw std::invalid_argument("Use nx >= 5 and ny >= 5.");
    }
    if (p.steps <= 0) {
        throw std::invalid_argument("Use steps > 0.");
    }
    if (p.tau <= 0.5) {
        throw std::invalid_argument("Use tau > 0.5 for positive lattice viscosity.");
    }
    if (p.saveEvery <= 0) {
        throw std::invalid_argument("Use saveEvery > 0.");
    }
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
            const int n = id(x, y, p.nx);

            double r = 0.0;
            double mx = 0.0;
            double my = 0.0;

            for (int k = 0; k < 9; ++k) {
                r  += f[n][k];
                mx += f[n][k] * cx[k];
                my += f[n][k] * cy[k];
            }

            rho[n] = r;
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
    const double omega = 1.0 / p.tau;

    for (int y = 1; y < p.ny - 1; ++y) {
        for (int x = 0; x < p.nx; ++x) {
            const int n = id(x, y, p.nx);

            for (int k = 0; k < 9; ++k) {
                const double feq = equilibrium(k, rho[n], ux[n], uy[n]);

                // Guo forcing for a streamwise body force. Here Fy = 0.
                const double cu = cx[k] * ux[n] + cy[k] * uy[n];
                const double forceTerm = w[k] * (1.0 - 0.5 * omega)
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
            const int n = id(x, y, p.nx);

            for (int k = 0; k < 9; ++k) {
                const int x2 = (x + cx[k] + p.nx) % p.nx;
                const int y2 = y + cy[k];

                // Periodic left/right boundaries and bounce-back at the walls.
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
    const std::vector<double>& rho,
    const std::vector<double>& ux,
    const std::vector<double>& uy,
    const Params& p
) {
    std::ofstream file("results/velocity_field.csv");
    file << "x,y,rho,ux,uy,speed\n";

    for (int y = 1; y < p.ny - 1; ++y) {
        for (int x = 0; x < p.nx; ++x) {
            const int n = id(x, y, p.nx);
            const double speed = std::sqrt(ux[n] * ux[n] + uy[n] * uy[n]);
            file << x << ',' << y << ',' << rho[n] << ',' << ux[n] << ',' << uy[n]
                 << ',' << speed << '\n';
        }
    }
}

void saveCenterProfile(
    const std::vector<double>& ux,
    const Params& p
) {
    std::ofstream file("results/centerline_profile.csv");
    file << "y,lbm_ux,analytical_ux\n";

    const int x = p.nx / 2;
    const double nu = viscosity(p.tau);

    // Effective channel height for halfway bounce-back walls.
    const double height = static_cast<double>(p.ny - 2);

    for (int y = 1; y < p.ny - 1; ++y) {
        const int n = id(x, y, p.nx);
        const double wallNormalPosition = y - 0.5;
        const double analytical = p.forceX * wallNormalPosition * (height - wallNormalPosition) / (2.0 * nu);

        file << y << ',' << ux[n] << ',' << analytical << '\n';
    }
}

void saveRunInfo(const Params& p) {
    std::ofstream file("results/run_info.txt");
    file << "LBM Poiseuille flow, D2Q9, BGK\n";
    file << "nx = " << p.nx << '\n';
    file << "ny = " << p.ny << '\n';
    file << "steps = " << p.steps << '\n';
    file << "saveEvery = " << p.saveEvery << '\n';
    file << "tau = " << p.tau << '\n';
    file << "viscosity = " << viscosity(p.tau) << '\n';
    file << "forceX = " << p.forceX << '\n';
}

void printRunSummary(const Params& p) {
    std::cout
        << "Running D2Q9 LBM Poiseuille flow\n"
        << "  nx        = " << p.nx << '\n'
        << "  ny        = " << p.ny << '\n'
        << "  steps     = " << p.steps << '\n'
        << "  tau       = " << p.tau << '\n'
        << "  viscosity = " << viscosity(p.tau) << '\n'
        << "  forceX    = " << p.forceX << '\n'
        << "  saveEvery = " << p.saveEvery << "\n\n";
}

int main(int argc, char** argv) {
    Params p;

    try {
        const bool shouldRun = readCommandLine(argc, argv, p);
        if (!shouldRun) {
            return 0;
        }
        validateParameters(p);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }

    std::filesystem::create_directories("results");

    const int totalCells = p.nx * p.ny;

    std::vector<Cell> f(totalCells);
    std::vector<Cell> afterCollision(totalCells);
    std::vector<Cell> fNew(totalCells);

    std::vector<double> rho(totalCells, 1.0);
    std::vector<double> ux(totalCells, 0.0);
    std::vector<double> uy(totalCells, 0.0);
    std::vector<double> oldUx(totalCells, 0.0);

    for (int y = 0; y < p.ny; ++y) {
        for (int x = 0; x < p.nx; ++x) {
            const int n = id(x, y, p.nx);
            for (int k = 0; k < 9; ++k) {
                f[n][k] = equilibrium(k, 1.0, 0.0, 0.0);
            }
        }
    }

    std::ofstream convergence("results/convergence.csv");
    convergence << "step,max_change\n";

    printRunSummary(p);

    for (int step = 1; step <= p.steps; ++step) {
        calculateRhoAndVelocity(f, rho, ux, uy, p);
        collide(f, afterCollision, rho, ux, uy, p);
        streamAndBounce(afterCollision, fNew, p);
        f.swap(fNew);

        if (step % p.saveEvery == 0 || step == 1 || step == p.steps) {
            calculateRhoAndVelocity(f, rho, ux, uy, p);

            double maxChange = 0.0;
            for (int y = 1; y < p.ny - 1; ++y) {
                for (int x = 0; x < p.nx; ++x) {
                    const int n = id(x, y, p.nx);
                    maxChange = std::max(maxChange, std::abs(ux[n] - oldUx[n]));
                    oldUx[n] = ux[n];
                }
            }

            convergence << step << ',' << maxChange << '\n';
            std::cout << "step " << step << "   max ux change = " << maxChange << '\n';
        }
    }

    calculateRhoAndVelocity(f, rho, ux, uy, p);
    saveVelocityField(rho, ux, uy, p);
    saveCenterProfile(ux, p);
    saveRunInfo(p);

    std::cout << "\nDone. Results are in the results folder.\n";
    return 0;
}
