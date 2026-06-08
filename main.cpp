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
    auto allGens = getAllGenerators(seed);
    
    std::vector<std::decay_t<decltype(allGens)>::value_type> activeGenerators;

    // ИНТЕРАКТИВНОЕ МЕНЮ ВЫБОРА ГПСЧ
    std::cout << "====================================================================\n";
    std::cout << " НАСТРОЙКА ИССЛЕДОВАТЕЛЬСКОГО СТЕНДА: ВЫБОР АКТИВНЫХ ГЕНЕРАТОРОВ\n";
    std::cout << "====================================================================\n\n";
    for (size_t i = 0; i < allGens.size(); ++i) {
        std::cout << i + 1 << ". " << allGens[i]->getName() << "\n";
    }
    std::cout << "0. СЕРИЙНЫЙ РЕЖИМ (Выбрать весь пул из " << allGens.size() << " генераторов)\n";
    std::cout << ">> ";
    
    int genChoice;
    std::cin >> genChoice;

    if (genChoice == 0) {
        activeGenerators = std::move(allGens);
        std::cout << "\n[Режим]: Серийное тестирование всего пула генераторов.\n";
    } 
    else if (genChoice > 0 && genChoice <= static_cast<int>(allGens.size())) {
        activeGenerators.push_back(std::move(allGens[genChoice - 1]));
        std::cout << "\n[Режим]: Одиночное тестирование генератора: " << activeGenerators.back()->getName() << "\n";
    } 
    else {
        std::cout << "Неверный выбор! По умолчанию активирован серийный режим.\n";
        activeGenerators = std::move(allGens);
    }

    system("pause");
    system("cls"); // очистка экрана перед запуском конвейера

    int actionChoice = -1;
    while (true) {
        std::cout << "====================================================================\n";
        std::cout << "  ПРОГРАММНЫЙ КОМПЛЕКС КУРСОВОГО ПРОЕКТА: BENCHMARK SUITE ГПСЧ\n";
        std::cout << "  Активных генераторов в конвейере: " << activeGenerators.size() << "\n";
        std::cout << "====================================================================\n\n";
        std::cout << "Выберите тест или действие:\n";
        std::cout << "1. Генерация сырых .bin файлов\n";
        std::cout << "2. Вычисление числа Пи (Монте-Карло)\n";
        std::cout << "3. Интегрирование функции (Монте-Карло)\n";
        std::cout << "4. Случайные блуждания (Coin Walk)\n";
        std::cout << "5. Оценка стоимости опционов по модели Блэка-Шоулза\n";
        std::cout << "0. Выход\n";
        std::cout << ">> ";
        std::cin >> actionChoice;

        if (actionChoice == 0) {
            std::cout << "Завершение работы программы..." << std::endl;
            break;
        }

        if (actionChoice < 1 || actionChoice > 5) {
            std::cout << "Неверный выбор! Попробуйте снова." << std::endl;
            continue;
        }

        if (actionChoice == 1) {
            int subGenChoice = 0; // по умолчанию 0 экспортировать все, что есть в конвейере

            // если в конвейере больше одного генератора, предлагаем выбрать конкретный
            if (activeGenerators.size() > 1) {
                std::cout << "\n--- ДОСТУПНЫЕ ГЕНЕРАТОРЫ ДЛЯ ЭКСПОРТА ---\n";
                for (size_t i = 0; i < activeGenerators.size(); ++i) {
                    std::cout << i + 1 << ". " << activeGenerators[i]->getName() << "\n";
                }
                std::cout << "0. Сгенерировать файлы для ВСЕХ активных генераторов\n";
                std::cout << ">> Выберите вариант: ";
                std::cin >> subGenChoice;
            }

            if (subGenChoice < 0 || subGenChoice > static_cast<int>(activeGenerators.size())) {
                std::cout << "Неверный выбор генератора! Возврат в главное меню.\n";
                system("pause");
                system("cls");
                continue;
            }

            std::cout << "\n[Формат данных]\n1. Целые числа (uint32) - для Dieharder/NIST\n2. Дробные числа (double) - для TestU01\n>> ";
            int typeChoice; std::cin >> typeChoice;
            uint64_t count; std::cout << "Введите количество чисел (например, 20000000): "; std::cin >> count;
            
            std::cout << "\n--- СЕРИЙНЫЙ ЭКСПОРТ ДАННЫХ В ФАЙЛЫ ---\n";
            
            if (subGenChoice == 0) {
                // прогоняем весь текущий конвейер (один или все девять)
                for (const auto& g : activeGenerators) {
                    std::string fname = std::string(g->getName()) + "_data.bin";
                    fname.erase(std::remove(fname.begin(), fname.end(), ' '), fname.end());
                    generateFile(*g, fname, count, (typeChoice == 2));
                }
            } else {
                // берем строго выбранный элемент (смещая индекс на 1)
                const auto& g = activeGenerators[subGenChoice - 1];
                std::string fname = std::string(g->getName()) + "_data.bin";
                fname.erase(std::remove(fname.begin(), fname.end(), ' '), fname.end());
                generateFile(*g, fname, count, (typeChoice == 2));
            }
        }
        // вычисление Пи
        else if (actionChoice == 2) {
            int N_runs; std::cout << "Введите количество итераций (N) [рекомендуется 10000000]: "; std::cin >> N_runs;
            std::cout << "\n--- [МЕНЕДЖЕР ТЕСТОВ: ВЫЧИСЛЕНИЕ ЧИСЛА ПИ] ---\n";
            for (const auto& g : activeGenerators) {
                Experiments::runPiExperiment(*g, N_runs);
            }
        } 
        // интегрирование функции
        else if (actionChoice == 3) {
            double a, b;
            int N_runs;
            std::string expression_string;
            std::cout << "Введите математическую функцию (например, sin(x)): ";
            std::cin >> std::ws;
            std::getline(std::cin, expression_string);

            std::cout << "Введите нижний предел интегрирования (a): "; std::cin >> a;
            std::cout << "Введите верхний предел интегрирования (b): "; std::cin >> b;
            std::cout << "Введите количество итераций (N): "; std::cin >> N_runs;
            
            std::cout << "\n--- [МЕНЕДЖЕР ТЕСТОВ: ИНТЕГРИРОВАНИЕ] ---\n";
            for (const auto& g : activeGenerators) {
                runIntegrationSimulation(*g, N_runs, a, b, expression_string); 
            }
        }
        // случайные блуждания
        else if (actionChoice == 4) {
            int T_coin, N_runs;
            std::cout << "Введите границу T (например, 10): "; std::cin >> T_coin;
            std::cout << "Введите количество итераций (N): "; std::cin >> N_runs;
            
            std::cout << "\n--- [МЕНЕДЖЕР ТЕСТОВ: СЛУЧАЙНЫЕ БЛУЖДАНИЯ COIN WALK] ---\n";
            for (const auto& g : activeGenerators) {
                runCoinSimulation(*g, N_runs, T_coin);
            }
        } 
        // модель Блэка-Шоулза
        else if (actionChoice == 5) {
            uint64_t bsIters; std::cout << "Введите количество траекторий (рекомендуется 10000000): "; std::cin >> bsIters;
            
            std::cout << "\n--- [МЕНЕДЖЕР ТЕСТОВ: МОДЕЛЬ БЛЭКА-ШОУЛЗА] ---\n";
            for (const auto& g : activeGenerators) {
                Experiments::runBlackScholesExperiment(*g, bsIters);
            }
        }

        std::cout << "\nТестирование группы алгоритмов успешно завершено.\n";
        system("pause");
        system("cls"); 
    }

    return 0;
}