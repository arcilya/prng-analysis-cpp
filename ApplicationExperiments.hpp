#ifndef APPLICATION_EXPERIMENTS_HPP
#define APPLICATION_EXPERIMENTS_HPP

#include <iostream>
#include <chrono>
#include <cmath>
#include <iomanip>
#include "Generator.hpp"

namespace Experiments {
    const double PI_REF = 3.14159265358979323846;

    void runPiExperiment(IGenerator& gen, uint64_t iterations) {
        std::cout << "\n[Эксперимент]: Вычисление Пи (Монте-Карло)" << std::endl;
        std::cout << "Используемый ГПСЧ: " << gen.getName() << std::endl;
        
        uint64_t pointsInCircle = 0;
        
        // Замеряем время начала
        auto start = std::chrono::high_resolution_clock::now();

        for (uint64_t i = 0; i < iterations; ++i) {
            double x = gen.nextDouble(); // Координата x от 0 до 1
            double y = gen.nextDouble(); // Координата y от 0 до 1
            
            // попадает ли точка в четверть круга 
            if (x * x + y * y <= 1.0) {
                pointsInCircle++;
            }
        }

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> diff = end - start;

        // Формула: (Точки в круге / Всего точек) = (Площадь круга / Площадь квадрата)
        // Пи = 4 * (В круге / Всего)
        double calculatedPi = 4.0 * static_cast<double>(pointsInCircle) / static_cast<double>(iterations);
        double mips = (static_cast<double>(iterations) / diff.count()) / 1e6;

        std::cout << std::fixed << std::setprecision(10);
        std::cout << "Вычисленное Пи:  " << calculatedPi << std::endl;
        std::cout << "Эталонное Пи:    " << PI_REF << std::endl;
        std::cout << "Погрешность:     " << std::abs(calculatedPi - PI_REF) << std::endl;
        std::cout << "Время расчета:   " << diff.count() << " сек." << std::endl;
        std::cout << "Скорость генерации: " << mips << " млн точек/сек" << std::endl;
        std::cout << "--------------------------------------------" << std::endl;
    }
}

#endif