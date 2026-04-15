#include <iostream>
#include <iomanip>
#include "PCG32.hpp"

int main() {
    // Первый параметр -  seed, второй - выбор последовательности (inc)
    PCG32 gen(42, 54321);

    std::cout << "Testing " << gen.getName() << std::endl;

    // 1. Проверка целых чисел (uint64_t)
    std::cout << "\nFirst 5 integers:" << std::endl;
    for (int i = 0; i < 5; ++i) {
        std::cout << "  " << gen.next() << std::endl;
    }

    // 2. Проверка дробных чисел от 0.0 до 1.0
    std::cout << "\nFirst 5 doubles (0.0 to 1.0):" << std::endl;
    for (int i = 0; i < 5; ++i) {
        std::cout << "  " << std::fixed << std::setprecision(6) << gen.nextDouble() << std::endl;
    }

    // 3. Проверка смены seed
    std::cout << "\nResetting seed to 42..." << std::endl;
    gen.setSeed(42);
    std::cout << "First integer after reset: " << gen.next() << std::endl;

    return 0;
}