#ifndef XORSHIFT_PLUS_HPP
#define XORSHIFT_PLUS_HPP

#include <cstdint>
#include "IGenerator.hpp"

class XorShift128Plus : public IGenerator {
private:
    uint64_t s[2];

public:
    explicit XorShift128Plus(uint64_t seed = 123456789ULL) {
        setSeed(seed);
    }

    uint64_t next() override {
        uint64_t s1 = s[0];
        const uint64_t s0 = s[1];
        const uint64_t result = s1 + s0;
        
        s[0] = s0;
        s1 ^= s1 << 23; 
        s[1] = s1 ^ s0 ^ (s1 >> 18) ^ (s0 >> 5); 
        
        return result;
    }

    void setSeed(uint64_t seed) override {
        s[0] = seed;
        s[1] = 0x123456789ABCDEF0ULL; 

        for (int i = 0; i < 16; ++i) next();
    }

    /**
     * Функция jump. 
     * Эквивалентна 2^64 вызовам next()
     */
    void jump() {
        static const uint64_t JUMP[] = { 0x8a5cd789635d2dff, 0x121fd2155c472f96 };

        uint64_t s0 = 0;
        uint64_t s1 = 0;
        for (int i = 0; i < 2; i++) {
            for (int b = 0; b < 64; b++) {
                if (JUMP[i] & (1ULL << b)) {
                    s0 ^= s[0];
                    s1 ^= s[1];
                }
                next();
            }
        }
        s[0] = s0;
        s[1] = s1;
    }

    const char* getName() const override {
        return "XorShift128+";
    }
};

#endif