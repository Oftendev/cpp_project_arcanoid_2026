#include "Block.hpp"
#include "Ball.hpp"
#include "Arcanoid.hpp"
#include "RandomGenerator.hpp"

Block::Block() : alive(true) {}

void Block::setPosition(const sf::Vector2f& p, const sf::Vector2f& s) {
    pos = p;
    size = s;
}

sf::FloatRect Block::getRect() const {
    return sf::FloatRect(pos.x, pos.y, size.x, size.y);
}

void Block::draw(sf::RenderWindow& window) {
    sf::RectangleShape blockRect(size);
    blockRect.setPosition(pos);
    blockRect.setOutlineColor(sf::Color::Black);
    blockRect.setOutlineThickness(-2.0f);
    blockRect.setFillColor(getColor());
    window.draw(blockRect);
    
    std::string sym = getSymbol();
    if (!sym.empty()) {
        sf::Text blockText;
        blockText.setFont(Arcanoid::getFont());
        blockText.setCharacterSize(static_cast<unsigned>(size.y * 0.6f));
        blockText.setString(sym);
        blockText.setFillColor(sf::Color::Black);
        sf::FloatRect tb = blockText.getLocalBounds();
        blockText.setOrigin(tb.left + tb.width/2, tb.top + tb.height/2);
        blockText.setPosition(pos.x + size.x/2, pos.y + size.y/2);
        window.draw(blockText);
    }
}

void BonusBlock::onDestroy(Arcanoid& game) {
    int r = RandomGenerator::getInstance().randomInRange(0, 6);
    game.spawnBonus(getRect().getPosition(), r);
    game.addScore(5);
}

// NormalBlock
sf::Color NormalBlock::getColor() const { return sf::Color(124, 252, 0); };
std::string NormalBlock::getSymbol() const { return ""; };
void NormalBlock::onHit(Ball& ball) { kill(); };

// HardenedBlock
HardenedBlock::HardenedBlock() {
    health = RandomGenerator::getInstance().randomInRange(2, 4);
}

sf::Color HardenedBlock::getColor() const {
    float dark = 1.0f - (health - 1) * 0.1f;
    return sf::Color(static_cast<sf::Uint8>(124 * dark), static_cast<sf::Uint8>(252 * dark), 0);
}

void HardenedBlock::draw(sf::RenderWindow& window) {
    sf::RectangleShape blockRect(size);
    blockRect.setPosition(pos);
    blockRect.setOutlineColor(sf::Color::Black);
    blockRect.setOutlineThickness(-2.0f*health); // Утолщение границы
    blockRect.setFillColor(getColor());
    window.draw(blockRect);
    
    std::string sym = getSymbol();
    if (!sym.empty()) {
        sf::Text blockText;
        blockText.setFont(Arcanoid::getFont());
        blockText.setCharacterSize(static_cast<unsigned>(size.y * 0.6f));
        blockText.setString(sym);
        blockText.setFillColor(sf::Color::Black);
        sf::FloatRect tb = blockText.getLocalBounds();
        blockText.setOrigin(tb.left + tb.width/2, tb.top + tb.height/2);
        blockText.setPosition(pos.x + size.x/2, pos.y + size.y/2);
        window.draw(blockText);
    }
}

std::string HardenedBlock::getSymbol() const { return std::to_string(health); };
void HardenedBlock::onHit(Ball& ball) {
    health -= 1;
    if (health <= 0) kill();
}

// BonusBlock
sf::Color BonusBlock::getColor() const { return sf::Color(255, 215, 0); };
std::string BonusBlock::getSymbol() const { return "?"; };
void BonusBlock::onHit(Ball& ball) { kill(); };

// SpeedUpBlock
sf::Color SpeedUpBlock::getColor() const { return sf::Color(0, 191, 255); };
std::string SpeedUpBlock::getSymbol() const { return ">>"; };
void SpeedUpBlock::onHit(Ball& ball) { kill(); };
void SpeedUpBlock::onDestroy(Arcanoid& game) {
    game.speedUpBall();
    game.addScore(7);
}

// IndestructibleBlock
sf::Color IndestructibleBlock::getColor() const { return sf::Color(128, 128, 128); };
std::string IndestructibleBlock::getSymbol() const { return "#"; };
void IndestructibleBlock::onHit(Ball& ball) {};