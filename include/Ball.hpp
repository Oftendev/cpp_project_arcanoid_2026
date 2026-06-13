#pragma once

#include "GameObject.hpp"

class Paddle;

class Ball : public GameObject {
public:
    static constexpr float ORIGINAL_BALL_SPEED = 400.0f;
    Ball();
    // Обязательные для реализации методы из GameObject
    void update(float dt) override;
    void draw(sf::RenderWindow& window) override;
    sf::FloatRect getRect() const override;
    
    void init(float rad, float startX, float startY); // Спаун шарика
    void launch(float speed); // Запуск вверх со скоростью speed
    void resetToPaddle(const Paddle& paddle);
    void bounceFromPaddle(const Paddle& paddle); // Обработка отскока от платформы
    
    // Геттеры и сеттеры
    void setAttached(bool b) { attached = b; };
    bool isAttached() const { return attached; };
    void setPosition(float x, float y) {pos.x = x; pos.y = y; };
    void setVelocity(float vx, float vy) { velocity.x = vx; velocity.y = vy; };
    sf::Vector2f getVelocity() const { return velocity; };
    sf::Vector2f getPosition() const { return pos; };
    float getRadius() const { return radius; };

    // Для отскока
    void reflectX() { velocity.x = -velocity.x; };
    void reflectY() { velocity.y = -velocity.y; };
    
    // Методы для работы с таймером прилипания к платформе
    void updateStickTimer(float dt);
    void startStickTimer(float seconds);
    void stopStickTimer() { stickTimer = 0.0f; };
    bool isStickTimerActive() const { return stickTimer > 0.0f; };
    
private:
    sf::Vector2f pos;
    sf::Vector2f velocity;
    float radius;
    bool attached;
    float stickTimer;
    
};