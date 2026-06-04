#pragma once
#define _USE_MATH_DEFINES 
#include <iostream>
#include <vector>
#include <chrono>
#include <memory>
#include <cmath>
#include <iomanip>
#include <stdexcept>

#include "IGenerator.hpp"
#include "ModelPois.hpp"
#include "LCGGenerator.hpp"
#include "XorshiftGenerator.hpp"
#include "XorshiftPlusGenerator.hpp"
#include "XorshiftStarGenerator.hpp"
#include "MonteCarloGenerator.hpp"
#include "StdGenerator.hpp"
#include "exprtk.hpp"

void runIntegrationSimulation(IGenerator& gen, int N_runs, double a, double b) {
    auto start = std::chrono::high_resolution_clock::now();
    std::string expression_string = "sin(x)";

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