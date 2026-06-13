#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include "Paddle.hpp"
#include "Ball.hpp"

class Block;
class Bonus;

class Arcanoid {
public:
    Arcanoid(int w, int h);
    ~Arcanoid();
    
    // Базовые методы
    void handlePaddleInput(float dt);
    void handleEvent(const sf::Event& event);
    void update(float dt);
    void draw(sf::RenderWindow& window);
    
    // Метод спауна бонуса
    void spawnBonus(const sf::Vector2f& pos, int bonusType);
    // Методы для бонусов
    void expandPaddle();
    void shrinkPaddle();
    void speedUpBall();
    void slowDownBall();
    void stickBall();
    void activateBottomShield();
    void randomTurn();

    // Метод увеличения score
    void addScore(int delta) { score += delta; };
    // Для шрифта
    static sf::Font& getFont() { return font; };
    
private:
    int width, height;
    int score;
    bool isPaused, isGameOver, isVictory;
    bool bottomShieldActive;
    
    Paddle paddle;
    Ball ball;
    std::vector<std::unique_ptr<Block>> blocks; // Список всех блоков
    std::vector<std::unique_ptr<Bonus>> bonuses; // Список всех бонусов
    
    float originalBallSpeed;
    float paddleResizeTimer;
    float ballStickTimer;
    
    // Вспомогательные методы для работы
    void initializeBlocks();
    void checkCollisionWithWalls();
    void checkCollisionWithPaddle();
    void checkCollisionsWithBlocks();
    void checkBallLost();

    void updateBonuses(float dt);
    void resetBall();
    void switchPause();
    void restartGame();
    int countRemainingBlocks() const;
    void resetBonusEffects();
    
    static sf::Font font;
};