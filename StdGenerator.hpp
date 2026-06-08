#include <cstdlib>

#include "IGenerator.hpp"

class StdRandGenerator : public IGenerator {
public:
    StdRandGenerator(uint64_t seed = 12345) { setSeed(seed); }

    uint64_t next() override {
        return std::rand();
    }

    void setSeed(uint64_t seed) override {
        std::srand(static_cast<unsigned int>(seed));
    }

    double nextDouble() override {
        return static_cast<double>(next()) / 4294967296.0;
    }

    const char* getName() const override {
        return "std::rand";
    }
};