#ifndef GENERATOR_HPP
#define GENERATOR_HPP

#include <cstdint>

/**
 * Базовый интерфейс для всех генераторов (IGenerator).
 */
class IGenerator {
public:
    // Виртуальный деструктор (правильная очистка памяти)
    virtual ~IGenerator() = default;

    // Генерирует следующее 64-битное случайное число
    virtual uint64_t next() = 0;

    // Генерирует число в диапазоне [0.0, 1.0)
    // Общий метод для всех
    virtual double nextDouble() {
        // Деление на 2^64 для получения числа от 0 до 1
        return static_cast<double>(next()) / 18446744073709551616.0;
    }

    // Метод для установки начального значения
    virtual void setSeed(uint64_t seed) = 0;

    // Название генератора
    virtual const char* getName() const = 0;
};

#endif