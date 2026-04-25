#include <fstream>
#include <vector>
#include <iostream>

#include "../IGenerator.hpp"

void generateDataBinary(IGenerator& gen, const std::string& filename, size_t count) {
    std::ofstream outFile(filename, std::ios::out | std::ios::binary);

    for (size_t i = 0; i < count; ++i) {
        uint64_t value = gen.next();
        outFile.write(reinterpret_cast<const char*>(&value), sizeof(uint64_t));
    }

    outFile.close();
    std::cout << "Success, wrote " << count << " numbers to " << filename << std::endl;
}