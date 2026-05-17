#ifndef XORSHIFT_HPP
#define XORSHIFT_HPP

#include "IGenerator.hpp"

/**
 * Xorshift генератор
 */
class Xorshift64Generator : public IGenerator {
private:
    uint64_t state;
    
public:
    explicit Xorshift64Generator(uint64_t seed = 123456789) : state(seed) {
        if (state == 0) state = 1;  // состояние не должно быть нулевым
        // прогрев
        for (int i = 0; i < 10; ++i) {
            next();
        }
    }
    
    uint64_t next() override {
        state ^= state << 13;
        state ^= state >> 7;
        state ^= state << 17;
        return state;
    }
    
    void setSeed(uint64_t seed) override {
        state = seed;
        if (state == 0) state = 1;
        for (int i = 0; i < 10; ++i) {
            next();
        }
    }
    
    const char* getName() const override {
        return "XorShift64";
    }
};

/*
 * Xorshift128
 * Период: 2^128 - 1 (для 128-битного состояния)
 */
class Xorshift128Generator : public IGenerator {
private:
    uint32_t x, y, z, w;  // 128-битное состояние (4 × 32 бита)
    
public:
    explicit Xorshift128Generator(uint64_t seed = 123456789) {
        setSeed(seed);
    }
    
    /**
     * операции: t = x ^ (x << 11); x = y; y = z; z = w; w = w ^ (w >> 19) ^ t ^ (t >> 8)
     */
    uint64_t next() override {
        uint32_t t = x ^ (x << 11);
        
        x = y;
        y = z;
        z = w;
        w = w ^ (w >> 19) ^ t ^ (t >> 8);
        
        return static_cast<uint64_t>(w);
    }
    
    void setSeed(uint64_t seed) override {
        x = static_cast<uint32_t>(seed);
        y = static_cast<uint32_t>(seed >> 32) ^ 0xAAAAAAAA;
        z = static_cast<uint32_t>(seed * 1812433253 + 1);
        w = static_cast<uint32_t>(seed * 1664525 + 1013904223);
        
        if (x == 0) x = 1;
        if (y == 0) y = 2;
        if (z == 0) z = 3;
        if (w == 0) w = 4;
        
        for (int i = 0; i < 20; ++i) {
            next();
        }
    }
    
    const char* getName() const override {
        return "XorShift (Marsaglia)";
    }

    double nextDouble() {
        return static_cast<double>(next()) / 4294967296.0;
    }
};

#endif 