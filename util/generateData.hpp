#include <fstream>
#include <iostream>
#include <vector>

#include "../IGenerator.hpp"

void generateData(IGenerator& gen, const std::string& filename, size_t count, uint64_t m) {
    std::ofstream outFile(filename);
    outFile << "x,y\n";

    uint64_t current = gen.next();
    for (size_t i = 0; i < count; ++i) {
        uint64_t nextVal = gen.next();

        outFile << current << "," << nextVal << "\n";
        current = nextVal; // сдвигаемся на один шаг
    }
}