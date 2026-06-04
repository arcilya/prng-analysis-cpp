#define _USE_MATH_DEFINES 
#include <windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include <memory>
#include <chrono> 
#include <random> 
#include <iomanip>
#include <vector>
#include <cmath>

#include "PCG32.hpp"
#include "MT19937.hpp"
#include "ApplicationExperiments.hpp"

#include "Simulations.hpp"

// Ваша обертка для стандартного STL вихря
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
    uint64_t next() override { return engine(); }
    double nextDouble() override { return dist(engine); }
};

// Ваша функция записи в бинарный файл
void generateFile(IGenerator& gen, const std::string& filename, uint64_t count, bool asDouble) {
    std::ofstream ofs(filename, std::ios::binary);
    if (!ofs.is_open()) {
        std::cerr << "Ошибка открытия файла!" << std::endl;
        return;
    }
    std::cout << "[Запись] Генерация " << (asDouble ? "double [0,1)" : "uint32") 
              << " через " << gen.getName() << " -> " << filename << std::endl;

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
    std::cout << "Успешно за " << diff.count() << " сек. Скорость: " 
              << (count / diff.count()) / 1e6 << " млн/сек\n----------------------------------------" << std::endl;
}

// Единая фабрика: собирает ВСЕ 9 генераторов вашей команды
std::vector<std::unique_ptr<IGenerator>> getAllGenerators(uint64_t seed) {
    std::vector<std::unique_ptr<IGenerator>> generators;
    
    // 6 Генераторов коллеги
    generators.push_back(std::make_unique<StdRandGenerator>(seed));
    generators.push_back(std::make_unique<LinearCongruentialGenerator>(seed));
    generators.push_back(std::make_unique<Xorshift64Generator>(seed));
    generators.push_back(std::make_unique<Xorshift128Generator>(seed));
    generators.push_back(std::make_unique<XorShift128Plus>(seed));
    generators.push_back(std::make_unique<XorShift128Star>(seed));
    
    // 3 Ваших генератора
    auto pcg = std::make_unique<PCG32>(); pcg->setSeed(seed); generators.push_back(std::move(pcg));
    auto mt = std::make_unique<MT19937>(); mt->setSeed(seed); generators.push_back(std::move(mt));
    auto std_mt = std::make_unique<StdMT19937>(); std_mt->setSeed(seed); generators.push_back(std::move(std_mt));
    
    return generators;
}

int main() {
    SetConsoleOutputCP(65001); 
    SetConsoleCP(65001);

    uint64_t seed = 1234567;
    int actionChoice = -1;

    // Дефолтные параметры для симуляций коллеги
    double lambda = 2.0;
    double T_pois = 10.0;
    int T_coin = 10;
    double a = 0.0;
    double b = M_PI;

    while (true) {
        std::cout << "====================================================================\n";
        std::cout << "  ПРОГРАММНЫЙ КОМПЛЕКС КУРСОВОГО ПРОЕКТА: BENCHMARK SUITE ГПСЧ\n";
        std::cout << "====================================================================\n\n";
        std::cout << "Выберите категорию тестов:\n";
        std::cout << "1. ЭМПИРИЧЕСКИЙ АНАЛИЗ (Генерация сырых .bin файлов для NIST/TestU01)\n";
        std::cout << "2. ГЕОМЕТРИЧЕСКИЙ МОНТЕ-КАРЛО (Вычисление Пи + Интегрирование sin(x))\n";
        std::cout << "3. ДИСКРЕТНЫЕ СТОХАСТИЧЕСКИЕ ПРОЦЕССЫ (Процесс Пуассона + Блуждания Coin Walk)\n";
        std::cout << "4. ФИНАНСОВОЕ МОДЕЛИРОВАНИЕ (Оценка стоимости по модели Блэка-Шоулза)\n";
        std::cout << "0. Выход\n";
        std::cout << ">> ";
        std::cin >> actionChoice;

        if (actionChoice == 0) {
            std::cout << "Завершение работы программы..." << std::endl;
            break;
        }

        if (actionChoice < 1 || actionChoice > 4) {
            std::cout << "Неверный выбор! Попробуйте снова." << std::endl;
            continue;
        }

        // Генерируем чистый пул генераторов для выбранного эксперимента
        auto generators = getAllGenerators(seed);

        if (actionChoice == 1) {
            std::cout << "\n[Формат данных]\n1. Целые числа (uint32) - для Dieharder/NIST\n2. Дробные числа (double) - для TestU01\n>> ";
            int typeChoice; std::cin >> typeChoice;
            uint64_t count; std::cout << "Введите количество чисел (например, 20000000): "; std::cin >> count;
            
            std::cout << "\n--- СЕРИЙНЫЙ ЭКСПОРТ ДАННЫХ В ФАЙЛЫ ---\n";
            for (const auto& g : generators) {
                std::string fname = std::string(g->getName()) + "_data.bin";
                // Удаляем пробелы из имен файлов, чтобы ОС не ругалась
                fname.erase(std::remove(fname.begin(), fname.end(), ' '), fname.end());
                generateFile(*g, fname, count, (typeChoice == 2));
            }
        } 
        else if (actionChoice == 2) {
            int N_runs; std::cout << "Введите количество итераций (N) [рекомендуется 10000000]: "; std::cin >> N_runs;
            
            std::cout << "\n--- [МЕНЕДЖЕР ТЕСТОВ: ВЫЧИСЛЕНИЕ ЧИСЛА ПИ] ---\n";
            for (const auto& g : generators) {
                Experiments::runPiExperiment(*g, N_runs);
            }

            std::cout << "\n--- [МЕНЕДЖЕР ТЕСТОВ: ИНТЕГРИРОВАНИЕ sin(x)] ---\n";
            for (const auto& g : generators) {
                runIntegrationSimulation(*g, N_runs, a, b);
            }
        } 
        else if (actionChoice == 3) {
            int N_runs; std::cout << "Введите количество итераций (N): "; std::cin >> N_runs;
            
            std::cout << "\n--- [МЕНЕДЖЕР ТЕСТОВ: МОДЕЛЬ ПУАССОНА] ---\n";
            for (const auto& g : generators) {
                runPoissonSimulation(*g, N_runs, lambda, T_pois);
            }

            std::cout << "\n--- [МЕНЕДЖЕР ТЕСТОВ: СЛУЧАЙНЫЕ БЛУЖДАНИЯ COIN WALK] ---\n";
            for (const auto& g : generators) {
                runCoinSimulation(*g, N_runs, T_coin);
            }
        } 
        else if (actionChoice == 4) {
            uint64_t bsIters; std::cout << "Введите количество траекторий (рекомендуется 10000000): "; std::cin >> bsIters;
            
            std::cout << "\n--- [МЕНЕДЖЕР ТЕСТОВ: МОДЕЛЬ БЛЭКА-ШОУЛЗА] ---\n";
            for (const auto& g : generators) {
                Experiments::runBlackScholesExperiment(*g, bsIters);
            }
        }

        std::cout << "\nТестирование группы алгоритмов успешно завершено.\n";
        system("pause");
        system("cls"); // Очистка консоли для красоты интерфейса
    }

    return 0;
}