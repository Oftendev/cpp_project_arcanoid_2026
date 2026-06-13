#include "Ball.hpp"
#include "Paddle.hpp"
#include <cmath>

Ball::Ball() 
    : radius(10.0f), attached(true), stickTimer(0.0f) {
    pos = sf::Vector2f(0, 0);
    velocity = sf::Vector2f(0, 0);
}

void Ball::init(float rad, float startX, float startY) {
    radius = rad;
    pos.x = startX;
    pos.y = startY;
    velocity.x = 0.0f;
    velocity.y = 0.0f;
    attached = true;
    stickTimer = 0.0f;
}

void Ball::update(float dt) {
    if (!attached) {
        pos.x += velocity.x * dt;
        pos.y += velocity.y * dt;
    }
    updateStickTimer(dt);
}

void Ball::draw(sf::RenderWindow& window) {
    sf::CircleShape shape(radius);
    shape.setPosition(pos.x - radius, pos.y - radius);
    shape.setFillColor(sf::Color::White);
    shape.setOutlineColor(sf::Color::Black);
    shape.setOutlineThickness(-2.0f);
    window.draw(shape);
}

sf::FloatRect Ball::getRect() const {
    return sf::FloatRect(pos.x - radius, pos.y - radius, radius * 2, radius * 2);
}

void Ball::launch(float speed) {
    attached = false;
    velocity.x = 0.0f;
    velocity.y = -speed;
    stickTimer = 0.0f;
}

void Ball::resetToPaddle(const Paddle& paddle) {
    attached = true;
    pos.x = paddle.getRect().left + paddle.getWidth() / 2.0f;
    pos.y = paddle.getRect().top - radius;
    velocity.x = 0.0f;
    velocity.y = 0.0f;
    stickTimer = 0.0f;
}

void Ball::bounceFromPaddle(const Paddle& paddle) {
    if (velocity.y > 0) {
        pos.y = paddle.getRect().top - radius;
        float paddleCenterX = paddle.getRect().left + paddle.getWidth() / 2.0f;
        float offsetX = pos.x - paddleCenterX;
        float offset = offsetX / (paddle.getWidth() / 2.0f); // Отношение точки положения к краю платформы
        offset = std::clamp(offset, -1.0f, 1.0f); // Ограничиваем offset
        float angle = 1.22173f * offset; // 1.22173f радиан - 70 градусов
        float currentSpeed = std::hypotf(velocity.x, velocity.y);
        // Новая скорость
        velocity.x = currentSpeed * std::sin(angle);
        velocity.y = -currentSpeed * std::cos(angle);
    }
}

void Ball::updateStickTimer(float dt) {
    if (stickTimer > 0.0f) {
        stickTimer -= dt;
        if (stickTimer <= 0.0f && attached) {
            launch(Ball::ORIGINAL_BALL_SPEED);
        }
    }
}

void Ball::startStickTimer(float seconds) {
    attached = true;
    stickTimer = seconds;
}