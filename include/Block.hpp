#pragma once
#include "GameObject.hpp"

class Ball;
class Arcanoid;

class Block : public GameObject {
public:
    Block();
    virtual ~Block() = default;
    
    // Методы для обработки удара с шариком
    virtual void onHit(Ball& ball) = 0;
    sf::FloatRect getRect() const override;

    // Методы для отисовки блоков
    void draw(sf::RenderWindow& window) override;
    virtual sf::Color getColor() const = 0;
    virtual std::string getSymbol() const = 0;

    void update(float dt) override {}; // У блоков нет анимации
    virtual void onDestroy(Arcanoid& game) {}; // Для бонусных блоков
    virtual int getScoreValue() const { return 1; }; // Базовое начисление очков
    virtual bool isDestructible() const { return true; };

    // Геттеры и сеттеры
    void setPosition(const sf::Vector2f& p, const sf::Vector2f& s);
    bool isAlive() const override { return alive; };
    void kill() { alive = false; };
    virtual int getHealth() const { return 1; };
    virtual void setHealth(int h) {};

protected:
    bool alive;
    sf::Vector2f pos;
    sf::Vector2f size;
};

// NormalBlock
class NormalBlock : public Block {
public:
    sf::Color getColor() const override;
    std::string getSymbol() const override;
    void onHit(Ball& ball) override;
};

// HardenedBlock
class HardenedBlock : public Block {
public:
    HardenedBlock();
    void draw(sf::RenderWindow& window) override; // Т.к. в этом блоке должна быть другая толщина границы
    sf::Color getColor() const override;
    std::string getSymbol() const override;
    void onHit(Ball& ball) override;
    int getHealth() const override { return health; };
    void setHealth(int h) override { health = h; };
private:
    int health;
};

// BonusBlock
class BonusBlock : public Block {
public:
    sf::Color getColor() const override;
    std::string getSymbol() const override;
    void onHit(Ball& ball) override;
    void onDestroy(Arcanoid& game) override;
};

// SpeedUpBlock
class SpeedUpBlock : public Block {
public:
    sf::Color getColor() const override;
    std::string getSymbol() const override;
    void onHit(Ball& ball) override;
    void onDestroy(Arcanoid& game) override; // Эффект для него отличается (не спаунится бонус)
};

// IndestructibleBlock
class IndestructibleBlock : public Block {
public:
    sf::Color getColor() const override;
    std::string getSymbol() const override;
    void onHit(Ball& ball) override;
    int getScoreValue() const override { return 0; }; // Очки не должны начисляться
    bool isDestructible() const override { return false; };
};