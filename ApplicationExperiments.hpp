#ifndef APPLICATION_EXPERIMENTS_HPP
#define APPLICATION_EXPERIMENTS_HPP

#include <iostream>
#include <chrono>
#include <cmath>
#include <iomanip>
#include <algorithm> 
#include "Generator.hpp"

namespace Experiments {
    const double PI_REF = 3.14159265358979323846;
    // Преобразование Бокса-Мюллера для получения случайного числа с нормальным распределением N(0, 1) из равномерного [0, 1)
    inline double getNormal(IGenerator& gen) {
        double u1 = gen.nextDouble();
        double u2 = gen.nextDouble();
        // Используем одну из ветвей преобразования
        return std::sqrt(-2.0 * std::log(u1)) * std::cos(2.0 * PI_REF * u2);
    }

    //ЧИСЛО ПИ

    void runPiExperiment(IGenerator& gen, uint64_t iterations) {
        std::cout << "\n[Эксперимент]: Вычисление Пи (Монте-Карло)" << std::endl;
        std::cout << "Используемый ГПСЧ: " << gen.getName() << std::endl;
        
        uint64_t pointsInCircle = 0;
        
        auto start = std::chrono::high_resolution_clock::now();

        for (uint64_t i = 0; i < iterations; ++i) {
            double x = gen.nextDouble(); 
            double y = gen.nextDouble(); 
            
            if (x * x + y * y <= 1.0) {
                pointsInCircle++;
            }
        }

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> diff = end - start;

        double calculatedPi = 4.0 * static_cast<double>(pointsInCircle) / static_cast<double>(iterations);
        double mips = (static_cast<double>(iterations) / diff.count()) / 1e6;

        std::cout << std::fixed << std::setprecision(10);
        std::cout << "Вычисленное Пи:     " << calculatedPi << std::endl;
        std::cout << "Эталонное Пи:       " << PI_REF << std::endl;
        std::cout << "Погрешность:        " << std::abs(calculatedPi - PI_REF) << std::endl;
        std::cout << "Время расчета:      " << diff.count() << " сек." << std::endl;
        std::cout << "Скорость генерации: " << mips << " млн точек/сек" << std::endl;
        std::cout << "--------------------------------------------" << std::endl;
    }

    // МОДЕЛЬ БЛЭКА-ШОУЛЗА

    void runBlackScholesExperiment(IGenerator& gen, uint64_t iterations) {
        std::cout << "\n[Эксперимент]: Модель Блэка-Шоулза (Monte Carlo Option Pricing)" << std::endl;
        std::cout << "Используемый ГПСЧ: " << gen.getName() << std::endl;

        // Входные параметры модели
        double S0 = 100.0;    // Текущая цена актива
        double mu = 0.05;     // Безрисковая процентная ставка
        double sigma = 0.2;    // Волатильность (20%)
        double T = 1.0;        // Срок опциона (1 год)
        double X = 105.0;      // Цена страйк (исполнения)

        double sumPayoffs = 0.0;
        
        auto start = std::chrono::high_resolution_clock::now();

        for (uint64_t i = 0; i < iterations; ++i) {
            // 1. Генерируем нормальную величину epsilon
            double epsilon = getNormal(gen); 
            
            // 2. Рассчитываем цену ST к моменту экспирации
            double ST = S0 * std::exp((mu - 0.5 * sigma * sigma) * T + sigma * std::sqrt(T) * epsilon);
            
            // 3. Расчет выплаты (Payoff) для опциона Call: max(ST - X, 0)
            double payoff = std::max(ST - X, 0.0);
            sumPayoffs += payoff;
        }

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> diff = end - start;

        // Итоговые вычисления
        double averagePayoff = sumPayoffs / static_cast<double>(iterations);
        double callPrice = averagePayoff * std::exp(-mu * T); // Дисконтирование
        double mips = (static_cast<double>(iterations) / diff.count()) / 1e6;

        std::cout << std::fixed << std::setprecision(6);
        std::cout << "Средний Payoff:      " << averagePayoff << std::endl;
        std::cout << "Цена Call-опциона:   " << callPrice << std::endl;
        std::cout << "Время расчета:       " << diff.count() << " сек." << std::endl;
        std::cout << "Скорость:            " << mips << " млн траекторий/сек" << std::endl;
        std::cout << "--------------------------------------------" << std::endl;
    }
}

#endif