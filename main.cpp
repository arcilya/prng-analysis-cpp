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

// единая фабрика
std::vector<std::unique_ptr<IGenerator>> getAllGenerators(uint64_t seed) {
    std::vector<std::unique_ptr<IGenerator>> generators;
    
    generators.push_back(std::make_unique<StdRandGenerator>(seed));
    generators.push_back(std::make_unique<LinearCongruentialGenerator>(seed));
    generators.push_back(std::make_unique<Xorshift64Generator>(seed));
    generators.push_back(std::make_unique<Xorshift128Generator>(seed));
    generators.push_back(std::make_unique<XorShift128Plus>(seed));
    generators.push_back(std::make_unique<XorShift128Star>(seed));
    
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

    while (true) {
        std::cout << "====================================================================\n";
        std::cout << "  ПРОГРАММНЫЙ КОМПЛЕКС КУРСОВОГО ПРОЕКТА: BENCHMARK SUITE ГПСЧ\n";
        std::cout << "====================================================================\n\n";
        std::cout << "Выберите тест или действие:\n";
        std::cout << "1. Эмпирический анализ (генерация сырых .bin файлов)\n";
        std::cout << "2. Вычисление числа Пи (Монте-Карло)\n";
        std::cout << "3. Интегрирование функции (Монте-Карло)\n";
        std::cout << "4. Процесс Пуассона (блуждания)\n";
        std::cout << "5. Случайные блуждания (дискретные симметричные)\n";
        std::cout << "6. Оценка стоимости опционов по модели Блэка-Шоулза\n";
        std::cout << "0. Выход\n";
        std::cout << ">> ";
        std::cin >> actionChoice;

        if (actionChoice == 0) {
            std::cout << "Завершение работы программы..." << std::endl;
            break;
        }

        if (actionChoice < 1 || actionChoice > 6) {
            std::cout << "Неверный выбор! Попробуйте снова." << std::endl;
            continue;
        }

        auto generators = getAllGenerators(seed);

        // генерация файлов (с выбором конкретного генератора)
        if (actionChoice == 1) {
            std::cout << "\n--- ДОСТУПНЫЕ ГЕНЕРАТОРЫ ---\n";
            for (size_t i = 0; i < generators.size(); ++i) {
                std::cout << i + 1 << ". " << generators[i]->getName() << "\n";
            }
            std::cout << "0. Сгенерировать для всех генераторов\n";
            std::cout << ">> Выберите генератор: ";
            int genChoice; std::cin >> genChoice;

            std::cout << "\n[Формат данных]\n1. Целые числа (uint32) - для Dieharder/NIST\n2. Дробные числа (double) - для TestU01\n>> ";
            int typeChoice; std::cin >> typeChoice;
            uint64_t count; std::cout << "Введите количество чисел (например, 20000000): "; std::cin >> count;
            
            std::cout << "\n--- ЭКСПОРТ ДАННЫХ В ФАЙЛЫ ---\n";
            
            if (genChoice == 0) {
                for (const auto& g : generators) {
                    std::string fname = std::string(g->getName()) + "_data.bin";
                    fname.erase(std::remove(fname.begin(), fname.end(), ' '), fname.end());
                    generateFile(*g, fname, count, (typeChoice == 2));
                }
            } else if (genChoice > 0 && genChoice <= static_cast<int>(generators.size())) {
                auto& g = generators[genChoice - 1];
                std::string fname = std::string(g->getName()) + "_data.bin";
                fname.erase(std::remove(fname.begin(), fname.end(), ' '), fname.end());
                generateFile(*g, fname, count, (typeChoice == 2));
            } else {
                std::cout << "Ошибка: Неверно указан номер генератора.\n";
            }
        } 
        // вычисление Пи
        else if (actionChoice == 2) {
            int N_runs; std::cout << "Введите количество итераций (N) [рекомендуется 10000000]: "; std::cin >> N_runs;
            std::cout << "\n--- [МЕНЕДЖЕР ТЕСТОВ: ВЫЧИСЛЕНИЕ ЧИСЛА ПИ] ---\n";
            for (const auto& g : generators) {
                Experiments::runPiExperiment(*g, N_runs);
            }
        } 
        // интегрирование
        else if (actionChoice == 3) {
            double a, b;
            int N_runs;
            std::cout << "Введите нижний предел интегрирования (a), например 0.0: "; std::cin >> a;
            std::cout << "Введите верхний предел интегрирования (b), например 3.14159: "; std::cin >> b;
            std::cout << "Введите количество итераций (N) [рекомендуется 10000000]: "; std::cin >> N_runs;
            
            std::cout << "\n--- [МЕНЕДЖЕР ТЕСТОВ: ИНТЕГРИРОВАНИЕ] ---\n";
            for (const auto& g : generators) {
                runIntegrationSimulation(*g, N_runs, a, b);
            }
        } 
        // процесс Пуассона 
        else if (actionChoice == 4) {
            double lambda, T_pois;
            int N_runs;
            std::cout << "Введите lambda (например, 2.0): "; std::cin >> lambda;
            std::cout << "Введите T (поглощащую границу) (например, 10.0): "; std::cin >> T_pois;
            std::cout << "Введите количество итераций (N): "; std::cin >> N_runs;
            
            std::cout << "\n--- [МЕНЕДЖЕР ТЕСТОВ: МОДЕЛЬ ПУАССОНА] ---\n";
            for (const auto& g : generators) {
                runPoissonSimulation(*g, N_runs, lambda, T_pois);
            }
        } 
        // блуждания
        else if (actionChoice == 5) {
            int T_coin, N_runs;
            std::cout << "Введите T (поглощающую границу) (например, 10): "; std::cin >> T_coin;
            std::cout << "Введите количество итераций (N): "; std::cin >> N_runs;
            
            std::cout << "\n--- [МЕНЕДЖЕР ТЕСТОВ: СЛУЧАЙНЫЕ БЛУЖДАНИЯ COIN WALK] ---\n";
            for (const auto& g : generators) {
                runCoinSimulation(*g, N_runs, T_coin);
            }
        } 
        // Блэк-Шоулз
        else if (actionChoice == 6) {
            uint64_t bsIters; std::cout << "Введите количество траекторий (рекомендуется 10000000): "; std::cin >> bsIters;
            
            std::cout << "\n--- [МЕНЕДЖЕР ТЕСТОВ: МОДЕЛЬ БЛЭКА-ШОУЛЗА] ---\n";
            for (const auto& g : generators) {
                Experiments::runBlackScholesExperiment(*g, bsIters);
            }
        }

        std::cout << "\nВыполнение завершено.\n";
        system("pause");
        system("cls"); 
    }

    return 0;
}