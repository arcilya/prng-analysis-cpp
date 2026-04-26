// #include <iostream>
// #include <iomanip>
// #include <vector>
// #include <memory>
// #include "PCG32.hpp"
// #include "MT19937.hpp"

// // Функция для тестирования генераторов
// void testGenerator(IGenerator* gen) {
//     std::cout << "Testing: " << gen->getName() << std::endl;

//     // 1. Проверка целых чисел (uint64_t)
//     std::cout << "First 5 integers:" << std::endl;
//     for (int i = 0; i < 5; ++i) {
//         std::cout << "  " << gen->next() << std::endl;
//     }

//     // 2. Проверка дробных чисел от 0.0 до 1.0
//     std::cout << "\nFirst 5 doubles (0.0 to 1.0):" << std::endl;
//     for (int i = 0; i < 5; ++i) {
//         std::cout << "  " << std::fixed << std::setprecision(8) << gen->nextDouble() << std::endl;
//     }

//     // 3. Проверка детерминированности (сброс seed)
//     std::cout << "\nResetting seed to 12345..." << std::endl;
//     gen->setSeed(12345);
//     uint64_t first = gen->next();
    
//     gen->setSeed(12345);
//     uint64_t second = gen->next();

//     std::cout << "First try:  " << first << std::endl;
//     std::cout << "Second try: " << second << std::endl;
    
//     if (first == second) {
//         std::cout << "SUCCESS: Results are identical" << std::endl;
//     } else {
//         std::cout << "ERROR: Results differ!" << std::endl;
//     }
//     std::cout << std::endl;
// }



#include <iostream>
#include <fstream>
#include <string>
#include <memory>

#include "PCG32.hpp"
#include "MT19937.hpp"

void generateFile(IGenerator& gen, const std::string& filename, uint64_t count, bool asDouble) {
    std::ofstream ofs(filename, std::ios::binary);
    if (!ofs.is_open()) {
        std::cerr << "Ошибка открытия файла" << std::endl;
        return;
    }

    std::cout << "Генерация " << (asDouble ? "double [0,1)" : "uint32") << " в " << filename << "..." << std::endl;

    for (uint64_t i = 0; i < count; ++i) {
        if (asDouble) {
            double val = gen.nextDouble();
            ofs.write(reinterpret_cast<const char*>(&val), sizeof(val));
        } else {
            uint32_t val = static_cast<uint32_t>(gen.next());
            ofs.write(reinterpret_cast<const char*>(&val), sizeof(val));
        }

        if (i % 5000000 == 0 && i > 0) {
            std::cout << "Прогресс: " << i << " чисел записано..." << std::endl;
        }
    }

    ofs.close();
    std::cout << "Файл создан!!!!!!!!!" << std::endl;
}

int main() {
    int genChoice, typeChoice;
    uint64_t count;
    std::string filename;

    std::cout << "=== Генератор тестовых данных ===\n";
    std::cout << "1. PCG32\n2. Mersenne Twister\nВыберите генератор: ";
    std::cin >> genChoice;

    std::cout << "\n1. Целые числа (uint32) - для Rabbit/Alphabit/Dieharder/NIST\n";
    std::cout << "2. Дробные числа (double [0,1)) - для SmallCrush\n";
    std::cin >> typeChoice;

    std::cout << "\nВведите количество чисел (например, 51320000): ";
    std::cin >> count;

    std::cout << "Введите имя файла (например, data.bin): ";
    std::cin >> filename;

    std::unique_ptr<IGenerator> gen;
    if (genChoice == 1) gen = std::make_unique<PCG32>();
    else gen = std::make_unique<MT19937>();

    generateFile(*gen, filename, count, (typeChoice == 2));

    return 0;
}