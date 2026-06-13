#include "Paddle.hpp"
#include <algorithm>

Paddle::Paddle(): width(100.0f), height(10.0f), speed(400.0f), originalWidth(100.0f), resizeTimer(0.0f), leftPressed(false), rightPressed(false), screenWidth(800.0f) {
    pos = sf::Vector2f(0, 0);
}

void Paddle::init(float startX, float startY, float w, float h, float spd) {
    pos.x = startX;
    pos.y = startY;
    width = w;
    height = h;
    speed = spd;
    originalWidth = w;
    resizeTimer = 0.0f;
}

void Paddle::update(float dt) {
    if (leftPressed) pos.x -= speed * dt;
    if (rightPressed) pos.x += speed * dt;
    pos.x = std::clamp(pos.x, 0.0f, screenWidth - width);
    updateTimer(dt);
}

void Paddle::draw(sf::RenderWindow& window) {
    sf::RectangleShape rect(sf::Vector2f(width, height));
    rect.setPosition(pos);
    rect.setFillColor(sf::Color::White);
    rect.setOutlineColor(sf::Color::Black);
    rect.setOutlineThickness(-3.0f);
    window.draw(rect);
}

sf::FloatRect Paddle::getRect() const {
    return sf::FloatRect(pos.x, pos.y, width, height);
}

void Paddle::setWidth(float newWidth) {
    float centerX = pos.x + width / 2.0f;
    width = std::clamp(newWidth, 25.0f, 300.0f);
    pos.x = centerX - width / 2.0f;
    pos.x = std::clamp(pos.x, 0.0f, screenWidth - width);
}

void Paddle::resetToOriginalWidth() {
    setWidth(originalWidth);
    stopResizeTimer();
}

void Paddle::updateTimer(float dt) {
    if (resizeTimer > 0.0f) {
        resizeTimer -= dt;
        if (resizeTimer <= 0.0f) {
            resetToOriginalWidth();
        }
    }
}

