#include "RandomGenerator.hpp"

// Конструктор
RandomGenerator::RandomGenerator() {
    rng.seed(static_cast<unsigned int>(std::time(nullptr)));
}

RandomGenerator& RandomGenerator::getInstance() {
    static RandomGenerator instance;
    return instance;
}

int RandomGenerator::randomInRange(int a, int b) {
    std::uniform_int_distribution<int> dist(a, b);
    return dist(rng);
}

std::mt19937& RandomGenerator::getRng() { return rng; }