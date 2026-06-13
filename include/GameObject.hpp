#pragma once

#include <SFML/Graphics.hpp>

// Абстрактный класс всех игровых объектов
class GameObject {
public:
    virtual ~GameObject() = default;

    // Виртуальные методы для обновления и отрисовки
    virtual void update(float dt) = 0;
    virtual void draw(sf::RenderWindow& window) = 0;

    // Виртуальные методы для столкновений
    virtual sf::FloatRect getRect() const = 0;
    virtual bool isAlive() const {return true; };
};