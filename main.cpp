#include <windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include <memory>
#include <chrono> 
#include <random> 
#include <iomanip>

#include "PCG32.hpp"
#include "MT19937.hpp"
#include "ApplicationExperiments.hpp"

class StdMT19937 : public IGenerator {
private:
    std::mt19937 engine;
    std::uniform_real_distribution<double> dist{0.0, 1.0};
public:
    StdMT19937() {
        std::random_device rd;
        engine.seed(rd());
    }

    void setSeed(uint64_t seed) override {
        engine.seed(static_cast<uint32_t>(seed));
    }

    const char* getName() const override {
        return "Standard C++ MT19937 (STL)";
    }

    uint64_t next() override { 
        return engine(); 
    }

    double nextDouble() override { 
        return dist(engine); 
    }
};

// Функция для генерации бинарных данных в файл
void generateFile(IGenerator& gen, const std::string& filename, uint64_t count, bool asDouble) {
    std::ofstream ofs(filename, std::ios::binary);
    if (!ofs.is_open()) {
        std::cerr << "Ошибка открытия файла!" << std::endl;
        return;
    }

    std::cout << "\n[Запись] Генерация " << (asDouble ? "double [0,1)" : "uint32") 
              << " в файл: " << filename << std::endl;

    auto start = std::chrono::high_resolution_clock::now();

    for (uint64_t i = 0; i < count; ++i) {
        if (asDouble) {
            double val = gen.nextDouble();
            ofs.write(reinterpret_cast<const char*>(&val), sizeof(val));
        } else {
            uint32_t val = static_cast<uint32_t>(gen.next());
            ofs.write(reinterpret_cast<const char*>(&val), sizeof(val));
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = end - start;
    ofs.close();

    std::cout << "Файл успешно создан за " << diff.count() << " сек." << std::endl;
    std::cout << "Скорость записи: " << (count / diff.count()) / 1e6 << " млн чисел/сек" << std::endl;
}

int main() {
    // Настройка кодировки для корректного вывода кириллицы
    SetConsoleOutputCP(65001); 
    SetConsoleCP(65001);

    std::cout << "========================================================" << std::endl;
    std::cout << "   ПРОГРАММНЫЙ КОМПЛЕКС ИССЛЕДОВАНИЯ АЛГОРИТМОВ ГПСЧ    " << std::endl;
    std::cout << "========================================================" << std::endl;

    std::cout << "\nВыберите действие:" << std::endl;
    std::cout << "1. Генерация файла данных (для внешних тестов NIST/Dieharder)" << std::endl;
    std::cout << "2. Эксперимент: Вычисление числа Пи (Монте-Карло)" << std::endl;
    std::cout << "3. Эксперимент: Модель Блэка-Шоулза (в разработке)" << std::endl;
    std::cout << ">> ";
    int actionChoice;
    std::cin >> actionChoice;

    if (actionChoice < 1 || actionChoice > 3) {
        std::cout << "Неверный выбор действия!" << std::endl;
        system("pause");
        return 1;
    }

    std::cout << "\nВыберите генератор:" << std::endl;
    std::cout << "1. Авторский PCG32" << std::endl;
    std::cout << "2. Авторский Mersenne Twister (MT19937)" << std::endl;
    std::cout << "3. Эталонный std::mt19937 (C++ STL)" << std::endl;
    std::cout << ">> ";
    int genChoice;
    std::cin >> genChoice;

    std::unique_ptr<IGenerator> gen;
    switch (genChoice) {
        case 1: gen = std::make_unique<PCG32>(); break;
        case 2: gen = std::make_unique<MT19937>(); break;
        case 3: gen = std::make_unique<StdMT19937>(); break;
        default: 
            std::cout << "Генератор не выбран!" << std::endl; 
            system("pause");
            return 1;
    }

    // Выполнение выбранного действия
    if (actionChoice == 1) {
        // Логика записи в файл
        std::cout << "\nФормат данных:" << std::endl;
        std::cout << "1. Целые числа (uint32) - для Dieharder/NIST" << std::endl;
        std::cout << "2. Дробные числа (double [0,1)) - для TestU01" << std::endl;
        std::cout << ">> ";
        int typeChoice;
        std::cin >> typeChoice;

        uint64_t count;
        std::cout << "Введите количество чисел (например, 50000000): ";
        std::cin >> count;

        std::string filename;
        std::cout << "Введите имя файла (например, data.bin): ";
        std::cin >> filename;

        generateFile(*gen, filename, count, (typeChoice == 2));
    } 
    else if (actionChoice == 2) {
        // Логика числа Пи
        uint64_t piIters;
        std::cout << "\n[Эксперимент Пи] Введите количество точек (рекомендуется 100 000 000): ";
        std::cin >> piIters;
        
        Experiments::runPiExperiment(*gen, piIters);
    }
    else if (actionChoice == 3) {
        std::cout << "\n[!] Модель Блэка-Шоулза будет добавлена далее" << std::endl;
    }

    std::cout << "\n========================================================" << std::endl;
    std::cout << "Тестирование завершено." << std::endl;
    
    system("pause");

    return 0;
}