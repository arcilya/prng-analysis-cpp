#ifndef XORSHIFT_STAR_HPP
#define XORSHIFT_STAR_HPP

#include "IGenerator.hpp"

/**
 * Xorshift* генератор 
 * Период 2^64 - 1
 */
class XorShift64Star : public IGenerator {
private:
    uint64_t state;
    
    // подобранная константа
    static constexpr uint64_t MULTIPLIER = 0x2545F4914F6CDD1DULL;
    
public:
    explicit XorShift64Star(uint64_t seed = 123456789) : state(seed) {
        if (state == 0) state = 1;
        for (int i = 0; i < 20; ++i) {
            next();
        }
    }

    uint64_t next() override {
        state ^= state >> 12;
        state ^= state << 25;
        state ^= state >> 27;
        
        // нелинейное преобразование - умножение
        return state * MULTIPLIER;
    }
    
    void setSeed(uint64_t seed) override {
        state = seed;
        if (state == 0) state = 1;
        for (int i = 0; i < 20; ++i) {
            next();
        }
    }
    
    const char* getName() const override {
        return "XorShift64*";
    }
};

/**
 * Xorshift128* - версия с 128-битным состоянием
 * период 2^128 - 1
 */
class XorShift128Star : public IGenerator {
private:
    uint64_t state0, state1;
    
    // константы для умножения (разные для чётных/нечётных шагов)
    static constexpr uint64_t MULTIPLIER0 = 0x9E3779B97F4A7C15ULL;
    static constexpr uint64_t MULTIPLIER1 = 0xBB67AE8584CAA73BULL;
    
public:
    explicit XorShift128Star(uint64_t seed = 123456789) {
        setSeed(seed);
    }
    
    uint64_t next() override {
        uint64_t s1 = state0;
        uint64_t s0 = state1;
        state0 = s0;
        
        s1 ^= s1 << 23;
        s1 ^= s1 >> 17;
        s1 ^= s0;
        s1 ^= s0 >> 26;
        state1 = s1;
        
        return ((state0 + state1) * MULTIPLIER0) ^ (state1 * MULTIPLIER1);
    }
    
    void setSeed(uint64_t seed) override {
        uint64_t z = seed + 0x9E3779B97F4A7C15ULL;
        z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ULL;
        z = (z ^ (z >> 27)) * 0x94D049BB133111EBULL;
        state0 = z ^ (z >> 31);
        
        z = state0 + 0x9E3779B97F4A7C15ULL;
        z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ULL;
        z = (z ^ (z >> 27)) * 0x94D049BB133111EBULL;
        state1 = z ^ (z >> 31);
        
        if (state0 == 0 && state1 == 0) {
            state0 = 1;
        }
        
        for (int i = 0; i < 30; ++i) {
            next();
        }
    }
    
    const char* getName() const override {
        return "XorShift128*";
    }
};

#endif