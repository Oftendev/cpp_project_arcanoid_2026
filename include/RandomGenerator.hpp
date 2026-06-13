#pragma once
#include <ctime>
#include <random>

// Класс генератора рандомных числел (синглтон)
class RandomGenerator {
   public:
    // Метод доступа к единственному экземпляру объекта
    static RandomGenerator& getInstance();

    int randomInRange(int a, int b);
    
    // Метод получения движка rng
    std::mt19937& getRng();
   private:
    std::mt19937 rng;
    RandomGenerator();  // Приватный конструктор
};