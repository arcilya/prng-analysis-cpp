#include <iostream>
#include <iomanip>
#include <vector>
#include <chrono>
#include <random>

#include "LCGGenerator.hpp"
#include "util/generateDataTable.hpp"

int main() {
    LinearCongruentialGenerator lcg(7); 
    generateDataTable(lcg, "data.csv", 5000, 6075);
    return 0;
}