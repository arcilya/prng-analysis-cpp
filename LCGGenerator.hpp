#ifndef LCG_GENERATOR_HPP
#define LCG_GENERATOR_HPP

#include "IGenerator.hpp"
#include <string>

/**
 * Линейный конгруэнтный генератор (LCG)
 */
class LinearCongruentialGenerator : public IGenerator {
private:
    uint32_t state; 
    
    // параметры LCG (константы)
    static constexpr uint32_t a = 1664525;   // a
    static constexpr uint32_t c = 1013904223;          // c
    // m = 2 ** 32, но он не нужен, т.к. используем переполнение uint32_t
    
public:
    explicit LinearCongruentialGenerator(uint32_t seed = 1) : state(seed) {}
    
    uint64_t next() override {
        // X_{n+1} = (a * X_n + c) mod m
        state = (a * state + c) % 2147483647;
        return static_cast<uint64_t>(state);
    }
    
    void setSeed(uint64_t seed) override {
        state = static_cast<uint32_t>(seed);
    }
    
    const char* getName() const override {
        return "Linear Congruential Generator (LCG)";
    }

     double nextDouble() {
        // Деление на 2^32 для получения числа от 0 до 1
        return static_cast<double>(next()) / 4294967296.0;
    }
};

#endif