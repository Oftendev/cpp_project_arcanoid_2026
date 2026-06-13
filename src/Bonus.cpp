#include "Bonus.hpp"
#include "Arcanoid.hpp"

// Конструктор общего bonus
Bonus::Bonus() : alive(false), size(20.0f, 20.0f), velocity(0.0f, 200.0f), screenHeight(800.0f) {};

void Bonus::spawn(const sf::Vector2f& position) {
    pos = position;
    alive = true;
}

void Bonus::update(float dt) {
    pos.y += velocity.y * dt;
    if (pos.y > screenHeight) { alive = false; };
}

void Bonus::draw(sf::RenderWindow& window) {
    sf::RectangleShape bonusRect(size);
    bonusRect.setPosition(pos);
    bonusRect.setOutlineColor(sf::Color::Black);
    bonusRect.setOutlineThickness(-1.0f);
    bonusRect.setFillColor(getColor());
    window.draw(bonusRect);
    
    sf::Text bonusText;
    bonusText.setFont(Arcanoid::getFont());
    bonusText.setCharacterSize(static_cast<unsigned>(size.y * 0.6f));
    bonusText.setString(getSymbol());
    bonusText.setFillColor(sf::Color::Black);
    sf::FloatRect tb = bonusText.getLocalBounds();
    bonusText.setOrigin(tb.left + tb.width/2, tb.top + tb.height/2);
    bonusText.setPosition(pos.x + size.x/2, pos.y + size.y/2);
    window.draw(bonusText);
}

sf::FloatRect Bonus::getRect() const {
    return sf::FloatRect(pos.x, pos.y, size.x, size.y);
}

// Далее реализуем методы для всех бонусов:
void ExpandPaddleBonus::apply(Arcanoid& game) { game.expandPaddle(); };
sf::Color ExpandPaddleBonus::getColor() const { return sf::Color::Green; };
std::string ExpandPaddleBonus::getSymbol() const { return "+"; };

void ShrinkPaddleBonus::apply(Arcanoid& game) { game.shrinkPaddle(); };
sf::Color ShrinkPaddleBonus::getColor() const { return sf::Color::Red; };
std::string ShrinkPaddleBonus::getSymbol() const { return "-"; };

void SpeedUpBallBonus::apply(Arcanoid& game) { game.speedUpBall(); };
sf::Color SpeedUpBallBonus::getColor() const { return sf::Color(0, 191, 255); };
std::string SpeedUpBallBonus::getSymbol() const { return ">>"; };

void SlowDownBallBonus::apply(Arcanoid& game) { game.slowDownBall(); };
sf::Color SlowDownBallBonus::getColor() const { return sf::Color(128, 0, 128); };
std::string SlowDownBallBonus::getSymbol() const { return "<<"; };

void StickBonus::apply(Arcanoid& game) { game.stickBall(); };
sf::Color StickBonus::getColor() const { return sf::Color::White; };
std::string StickBonus::getSymbol() const { return "|"; };

// BottomShieldBonus
void BottomShieldBonus::apply(Arcanoid& game) { game.activateBottomShield(); };
sf::Color BottomShieldBonus::getColor() const { return sf::Color::White; };
std::string BottomShieldBonus::getSymbol() const { return "_"; };

void RandomTurnBonus::apply(Arcanoid& game) { game.randomTurn(); };
sf::Color RandomTurnBonus::getColor() const { return sf::Color::Magenta; };
std::string RandomTurnBonus::getSymbol() const { return "*"; };