#ifndef MT19937_HPP
#define MT19937_HPP

#include "IGenerator.hpp"
#include <cstdint>

class MT19937 : public IGenerator {
private:
    // Параметры алгоритма 
    static const int p = 624;         // Степень рекуррентности (N)
    static const int q = 397;         // Смещение (M)
    static const int w = 32;          // Размер машинного слова
    static const int r = 31;          // Количество бит для нижней маски

    uint32_t X[p];                    // Состояние ГПСЧ (массив ячеек)
    int n;                            // Текущий индекс (в статье  n)

    // Маски для Шага 1а и Шага 2 
    const uint32_t u_mask = 0x80000000; // Старший бит (w-r бит)
    const uint32_t h_mask = 0x7fffffff; // Остальные r бит
    const uint32_t a = 0x9908B0DF;      // Последняя строка матрицы A

    // Константы закалки (Шаг 4)
    const int u = 11;
    const int s = 7;
    const uint32_t b = 0x9D2C5680;
    const int t = 15;
    const uint32_t c = 0xEFC60000;
    const int l = 18;

    // Процедура рекурсивной генерации (Шаги 2-3)
    void twist() {
        for (int i = 0; i < p; i++) {
            // Шаг 2: Конкатенация (X_i^r | X_{i+1}^l)
            uint32_t Y = (X[i] & u_mask) | (X[(i + 1) % p] & h_mask);
            
            // Шаг 3: Вычисление нового значения X_i с матрицей A
            // (Y >> 1) ^ a, если младший бит Y равен 1
            X[i] = X[(i + q) % p] ^ (Y >> 1);
            if (Y & 1) {
                X[i] ^= a;
            }
        }
        n = 0;
    }

public:
    MT19937(uint32_t seed = 5489U) {
        setSeed(seed);
    }

    void setSeed(uint64_t seed) override {
        // Шаг 1б: Заполнение начальными значениями
        X[0] = static_cast<uint32_t>(seed);
        for (n = 1; n < p; n++) {
            // Стандартная формула инициализации 
            X[n] = (1812433253U * (X[n - 1] ^ (X[n - 1] >> 30)) + n);
        }
        n = p; // Чтобы первый вызов next() запустил twist()
    }

    uint64_t next() override {
        if (n >= p) {
            twist();
        }

        // Шаг 4: Процедура закалки
        uint32_t y_val = X[n++];
        
        y_val ^= (y_val >> u);
        y_val ^= (y_val << s) & b;
        y_val ^= (y_val << t) & c;
        y_val ^= (y_val >> l);

        return static_cast<uint64_t>(y_val);
    }

    double nextDouble() override {
        // Деление на 2^32
        return static_cast<double>(next()) / 4294967296.0;
    }

    const char* getName() const override {
        return "Mersenne Twister (MT19937)";
    }
};

#endif