#include <iostream>
#include <vector>
#include <functional>
#include "IGenerator.hpp" 


class IntegeralCalculation {
public:
    static double integrate(IGenerator& gen, 
                            std::function<double(double)> func, 
                            double a, 
                            double b, 
                            uint64_t n) {
        if (n == 0) return 0.0;

        double sum = 0.0;
        double range = b - a;

        for (uint64_t i = 0; i < n; ++i) {
            double x = a + gen.nextDouble() * range;
            sum += func(x);
        }

        return (sum / static_cast<double>(n)) * range;
    }
};