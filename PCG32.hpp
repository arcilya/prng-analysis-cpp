#ifndef PCG32_HPP
#define PCG32_HPP

#include "IGenerator.hpp"
#include <cstdint>

class PCG32 : public IGenerator {
private:
    uint64_t state;
    uint64_t inc; // поток (должен быть нечетным)

public:
    PCG32(uint64_t init_state = 0x853c49e6748fea9bULL, uint64_t init_inc = 0xda3e39cb94b95bdbULL) {
        // inc должен быть нечетным
        inc = (init_inc << 1u) | 1u;
        state = 0;
        next();
        state += init_state;
        next();
    }

    uint64_t next() override {
        uint64_t oldstate = state;
        
        // Обычный LCG шаг для обновления состояния
        state = oldstate * 6364136223846793005ULL + inc;

        // Xorshift + Rotation (XSH-RR)
        // Вычисляем результат на основе старого состояния
        uint32_t xorshifted = static_cast<uint32_t>(((oldstate >> 18u) ^ oldstate) >> 27u);
        uint32_t rot = static_cast<uint32_t>(oldstate >> 59u);
        
        // Циклический сдвиг (rotate right)
        uint32_t result = (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
        
        return static_cast<uint64_t>(result);
    }

    double nextDouble() override {
        return static_cast<double>(next()) / 4294967296.0;
    }

    void setSeed(uint64_t seed) override {
        state = seed + inc;
        next();
    }

    const char* getName() const override {
        return "PCG32 (Permuted Congruential Generator)";
    }
};

#endif