#pragma once
#include "GameObject.hpp"

class Arcanoid;

class Bonus : public GameObject {
public:
    Bonus();
    virtual ~Bonus() = default;
    
    // Методы из GameObjecct
    void update(float dt) override;
    void draw(sf::RenderWindow& window) override;
    sf::FloatRect getRect() const override;
    bool isAlive() const override { return alive; };
    void setScreenHeight(float h) { screenHeight = h; };

    // Методы, необходимые для реализации в конкрентых бонусах
    virtual void apply(Arcanoid& game) = 0;
    virtual sf::Color getColor() const = 0;
    virtual std::string getSymbol() const = 0;
    
    // Создание бонуса
    void spawn(const sf::Vector2f& position);
    
protected:
    bool alive;
    sf::Vector2f pos;
    sf::Vector2f size;
    sf::Vector2f velocity;
    float screenHeight;
};

// Далее просто перечисляем все конкретные классы бонусы:
class ExpandPaddleBonus : public Bonus {
public:
    void apply(Arcanoid& game) override;
    sf::Color getColor() const override;
    std::string getSymbol() const override;
};

class ShrinkPaddleBonus : public Bonus {
public:
    void apply(Arcanoid& game) override;
    sf::Color getColor() const override;
    std::string getSymbol() const override;
};

class SpeedUpBallBonus : public Bonus {
public:
    void apply(Arcanoid& game) override;
    sf::Color getColor() const override;
    std::string getSymbol() const override;
};

class SlowDownBallBonus : public Bonus {
public:
    void apply(Arcanoid& game) override;
    sf::Color getColor() const override;
    std::string getSymbol() const override;
};

class StickBonus : public Bonus {
public:
    void apply(Arcanoid& game) override;
    sf::Color getColor() const override;
    std::string getSymbol() const override;
};

class BottomShieldBonus : public Bonus {
public:
    void apply(Arcanoid& game) override;
    sf::Color getColor() const override;
    std::string getSymbol() const override;
};

class RandomTurnBonus : public Bonus {
public:
    void apply(Arcanoid& game) override;
    sf::Color getColor() const override;
    std::string getSymbol() const override;
};