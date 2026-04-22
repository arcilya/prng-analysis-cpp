#include <iostream>
#include <iomanip>
#include <vector>
#include <memory>
#include "PCG32.hpp"
#include "MT19937.hpp"

// Функция для тестирования генераторов
void testGenerator(IGenerator* gen) {
    std::cout << "Testing: " << gen->getName() << std::endl;

    // 1. Проверка целых чисел (uint64_t)
    std::cout << "First 5 integers:" << std::endl;
    for (int i = 0; i < 5; ++i) {
        std::cout << "  " << gen->next() << std::endl;
    }

    // 2. Проверка дробных чисел от 0.0 до 1.0
    std::cout << "\nFirst 5 doubles (0.0 to 1.0):" << std::endl;
    for (int i = 0; i < 5; ++i) {
        std::cout << "  " << std::fixed << std::setprecision(8) << gen->nextDouble() << std::endl;
    }

    // 3. Проверка детерминированности (сброс seed)
    std::cout << "\nResetting seed to 12345..." << std::endl;
    gen->setSeed(12345);
    uint64_t first = gen->next();
    
    gen->setSeed(12345);
    uint64_t second = gen->next();

    std::cout << "First try:  " << first << std::endl;
    std::cout << "Second try: " << second << std::endl;
    
    if (first == second) {
        std::cout << "SUCCESS: Results are identical" << std::endl;
    } else {
        std::cout << "ERROR: Results differ!" << std::endl;
    }
    std::cout << std::endl;
}

int main() {
    PCG32 pcg(42, 54321);
    MT19937 mt(42);

    testGenerator(&pcg);
    testGenerator(&mt);

    return 0;
}