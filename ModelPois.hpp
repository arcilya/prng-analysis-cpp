#define _USE_MATH_DEFINES
#include <memory>
#include <cmath>
#include <algorithm>
#include <random>
#include <iostream>

#include "IGenerator.hpp"

class Pois {
    double lambda, T;
    IGenerator& gen;
public:
    Pois(IGenerator& _gen, double _lambda, double _T) : lambda(_lambda), T(_T), gen(_gen) {}

    int operator()() {
    std::poisson_distribution<int> distr(lambda);
    double X = 0;
    int n = 0;
    int multiplier = 1;

    while (std::abs(X) < T) {
        int step = distr(gen); 
        X += step * multiplier;


        multiplier *= -1;
        ++n;
    }
    return n;
}
};

class CoinWalk {
    int T;
    IGenerator& gen;
public:
    CoinWalk(IGenerator& _gen, int _T) : T(_T), gen(_gen) {}

    int operator()() {
        int X = 0;
        int n = 0;

        while (std::abs(X) < T) {
            int val = gen();
            int step = (val % 2 == 0) ? 1 : -1; 
            X += step;
            ++n;
        }
        return n;
    }
};