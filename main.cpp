#define _USE_MATH_DEFINES 
#include <iostream>
#include <vector>
#include <chrono>
#include <memory>
#include <map>
#include <cmath>
#include <iomanip>
#include <functional>
#include <stdexcept>

#include "IGenerator.hpp"
#include "ModelPois.hpp"
#include "LCGGenerator.hpp"
#include "XorshiftGenerator.hpp"
#include "XorshiftPlusGenerator.hpp"
#include "XorshiftStarGenerator.hpp"
#include "util/generateDataBinary.hpp"
#include "../exprtk.hpp"
#include "MonteCarloGenerator.hpp"
#include "StdGenerator.hpp"

void runIntegrationSimulation(IGenerator& gen, int N_runs, double a, double b) {
    auto start = std::chrono::high_resolution_clock::now();

    std::string expression_string;
    //std::cout << "Enter function with x (example: sin(x)): ";
    expression_string = "sin(x)";

    double x;
    exprtk::symbol_table<double> symbol_table;
    symbol_table.add_variable("x", x);
    symbol_table.add_constants();

    exprtk::expression<double> expression;
    expression.register_symbol_table(symbol_table);

    exprtk::parser<double> parser;
    if (!parser.compile(expression_string, expression)) {
        std::cerr << "Error in formula!" << std::endl;
        return;
    }

    auto target_func = [&](double current_x) {
        x = current_x; 
        return expression.value();
    };

    double mean = IntegeralCalculation::integrate(gen, target_func, a, b, N_runs);

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    std::cout << "[INTEGRATION] Generator: " << gen.getName() << "\n"
              << std::fixed << std::setprecision(15) << "  Mean integral : " << mean << "\n"
              << " Error " << mean - 2.0 << "\n"
              << "  Time          : " << duration << " ms\n"
              << "----------------------------------------" << std::endl;
}

void runPoissonSimulation(IGenerator& gen, int N, double lambda, double T) {
    auto start = std::chrono::high_resolution_clock::now();

    std::vector<int> res(N);
    Pois model(gen, lambda, T);

    double mean = 0.0;
    for (int i = 0; i < N; ++i) {
        res[i] = model();
        mean += res[i];
    }
    mean /= N;

    double disp = 0.0;
    for (int x : res) {
        disp += (x - mean) * (x - mean);
    }
    disp /= N;

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    std::cout << "[POISSON] Generator: " << gen.getName() << "\n"
              << std::fixed << std::setprecision(15) << "  Mean : " << mean << "\n"
              << std::fixed << std::setprecision(15) << "  Disp : " << disp << "\n"
              << "  Time : " << duration << " ms\n"
              << "----------------------------------------" << std::endl;
}

void runCoinSimulation(IGenerator& gen, int N, int T) {
    auto start = std::chrono::high_resolution_clock::now();

    CoinWalk model(gen, T);
    std::vector<int> results(N);
    
    double mean = 0.0;
    for (int i = 0; i < N; ++i) {
        results[i] = model();
        mean += results[i];
    }
    mean /= N;

    double disp = 0.0;
    for (int x : results) {
        disp += (x - mean) * (x - mean);
    }
    disp /= N;

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    std::cout << "[COIN WALK] Generator: " << gen.getName() << "\n"
              << std::fixed << std::setprecision(15) << "  Mean : " << mean << " (Theory: " << T * T << ")\n"
              << std::fixed << std::setprecision(15) << "  Disp : " << disp << "\n"
              << "  Time : " << duration << " ms\n"
              << "----------------------------------------" << std::endl;
}

std::vector<std::unique_ptr<IGenerator>> getGenerators(uint64_t seed) {
    std::vector<std::unique_ptr<IGenerator>> generators;
    generators.push_back(std::make_unique<StdRandGenerator>(seed));
    generators.push_back(std::make_unique<LinearCongruentialGenerator>(seed));
    generators.push_back(std::make_unique<Xorshift128Generator>(seed));
    generators.push_back(std::make_unique<Xorshift64Generator>(seed));
    generators.push_back(std::make_unique<XorShift128Plus>(seed));
    generators.push_back(std::make_unique<XorShift128Star>(seed));
    return generators;
}

int main() {
    uint64_t seed = 1234567;
    int choice = -1;

    // кол-во испытаний
    int N_runs = 10000000;
    
    double lambda = 2.0;
    double T_pois = 10.0;
    
    int T_coin = 10;
    
    int n_steps = 1000;
    double a = 0.0;
    double b = M_PI;

    while (true) {
        std::cout << "=== MAIN MENU ===\n";
        std::cout << "1. Run Integration\n";
        std::cout << "2. Run Poisson Process\n";
        std::cout << "3. Run Coin Walk\n";
        std::cout << "0. Exit\n";
        std::cout << "Select simulation: ";
        std::cin >> choice;

        if (choice == 0) {
            std::cout << "Exiting...\n";
            break;
        }

        std::cout << "Enter number of iterations (N) [default " << N_runs << "]: ";
        std::cin >> N_runs;

        auto generators = getGenerators(seed);

        std::cout << "\nStarting simulations with N = " << N_runs << "...\n\n";

        for (const auto& g : generators) {
            try {
                if (choice == 1) {
                    runIntegrationSimulation(*g, N_runs, a, b);
                } 
                else if (choice == 2) {
                    runPoissonSimulation(*g, N_runs, lambda, T_pois);
                } 
                else if (choice == 3) {
                    runCoinSimulation(*g, N_runs, T_coin);
                } 
                else {
                    std::cout << "Invalid choice!\n";
                    break;
                }
            } 
            catch (const std::exception& e) {
                std::cout << "Skipping generator '" << g->getName() 
                          << "' due to error: " << e.what() << "\n";
            }
        }
        std::cout << "\n";
    }

    return 0;
}