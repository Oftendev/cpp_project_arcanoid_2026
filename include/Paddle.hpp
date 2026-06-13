#pragma once

#include "GameObject.hpp"

class Paddle : public GameObject {
public:
    Paddle();
    // Обязательные для реализации методы из GameObject
    void update(float dt) override;
    void draw(sf::RenderWindow& window) override;
    sf::FloatRect getRect() const override;
    
    void init(float startX, float startY, float w, float h, float spd);
    void setInput(bool left, bool right) { leftPressed = left; rightPressed = right;};
    void setScreenWidth(float w) { screenWidth = w; };
    
    void setWidth(float newWidth);
    void resetToOriginalWidth();
    float getWidth() const { return width; };
    float getX() const { return pos.x; };
    sf::Vector2f getPosition() const { return pos; };

    
    void startResizeTimer(float seconds) {resizeTimer = seconds; };
    void stopResizeTimer() { resizeTimer = 0.0f; };
    void updateTimer(float dt);
    bool isResizeActive() const { return resizeTimer > 0.0f; };
    
    
private:
    sf::Vector2f pos;
    float width;
    float height;
    float speed;
    float originalWidth;
    float resizeTimer;
    bool leftPressed;
    bool rightPressed;
    float screenWidth;
};