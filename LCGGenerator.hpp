#ifndef LCG_GENERATOR_HPP
#define LCG_GENERATOR_HPP

#include "IGenerator.hpp"
#include <string>

/**
 * Линейный конгруэнтный генератор (LCG)
 */
class LinearCongruentialGenerator : public IGenerator {
private:
    uint64_t state; 
    
    // параметры LCG (константы)
    static constexpr uint64_t a = 1103515245;   // a
    static constexpr uint64_t c = 12345;          // c
    static constexpr uint64_t m = 2147483648;       // m = 2^31
    
public:
    explicit LinearCongruentialGenerator(uint64_t seed = 1) : state(seed) {}
    
    uint64_t next() override {
        // X_{n+1} = (a * X_n + c) mod m
        state = (a * state + c) % m;
        return state;
    }
    
    void setSeed(uint64_t seed) override {
        state = seed;
    }
    
    const char* getName() const override {
        return "Linear Congruential Generator (LCG)";
    }
};

#endif