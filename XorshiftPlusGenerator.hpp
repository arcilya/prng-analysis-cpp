#ifndef XORSHIFTPLUS_HPP
#define XORSHIFTPLUS_HPP

#include "IGenerator.hpp"

/**
 * Xorshift+ генератор 
 * Период 2^128 - 1
 */
class XorshiftPlus128Generator : public IGenerator {
private:
    uint64_t s[2];
    
public:
    explicit XorshiftPlus128Generator(uint64_t seed = 123456789) {
        setSeed(seed);
    }
    
    uint64_t next() override {
        uint64_t s1 = s[0];
        const uint64_t s0 = s[1];
        const uint64_t result = s0 + s1;
        
        s[0] = s0;
        s1 ^= s1 << 23;  // a
        s[1] = s1 ^ s0 ^ (s1 >> 18) ^ (s0 >> 5);  // b, c
        
        return result;
    }
    
    void setSeed(uint64_t seed) override {
        uint64_t z = seed + 0x9E3779B97F4A7C15ULL;
        z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ULL;
        z = (z ^ (z >> 27)) * 0x94D049BB133111EBULL;
        s[0] = z ^ (z >> 31);
        
        z = s[0] + 0x9E3779B97F4A7C15ULL;
        z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ULL;
        z = (z ^ (z >> 27)) * 0x94D049BB133111EBULL;
        s[1] = z ^ (z >> 31);
        
        if (s[0] == 0 && s[1] == 0) s[0] = 1;
        
        for (int i = 0; i < 20; ++i) next();
    }
    
    const char* getName() const override {
        return "Xorshift128+";
    }
};

#endif