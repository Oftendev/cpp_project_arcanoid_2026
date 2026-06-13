#include "Arcanoid.hpp"
#include "Block.hpp"
#include "Bonus.hpp"
#include "RandomGenerator.hpp"
#include <cmath>
#include <iostream>

sf::Font Arcanoid::font;

Arcanoid::Arcanoid(int w, int h) : width(w), height(h), score(0), isPaused(false), isGameOver(false), isVictory(false), bottomShieldActive(false), originalBallSpeed(400.0f), paddleResizeTimer(0.0f), ballStickTimer(0.0f) {
    if (!font.loadFromFile(PATH_TO_DATA "arial.ttf")) {
        std::cout << "Warning: Could not load font" << std::endl;
    }
    
    // Инициализируем платформу, шарик и блокт
    paddle.init((width - 100.0f) / 2.0f, height - 50.0f, 100.0f, 10.0f, 400.0f);
    paddle.setScreenWidth(width);
    ball.init(10.0f, paddle.getRect().left + paddle.getWidth() / 2.0f, paddle.getRect().top - 10.0f);
    initializeBlocks();
}

// Инициализация сетки блоков
void Arcanoid::initializeBlocks() {
    blocks.clear();
    // Параметры сетки блоков
    constexpr int blocksInRow = 10; // Кол-во блоков в ряду
    constexpr int rows = 5; // Кол-во рядов
    constexpr float offsetX = 50.0f; // Расстояние от границ до блоков
    constexpr float offsetY = 50.0f; // Отступ сверху до блоков
    constexpr float marginX = 5.0f; // Расстояние между блоками (x)
    constexpr float marginY = 5.0f; // Растояние между блоками (y)
    constexpr float blockHeight = 30.0f; // Высота блока
    float totalWidth = width - 2.0f * offsetX; // Общая ширина таблицы блоков
    float blockWidth = (totalWidth - marginX * static_cast<float>(blocksInRow - 1)) / static_cast<float>(blocksInRow); // Щирина блока
    // Вероятности появление разных типов блоков (в процентах) (сумма - 100%)
    constexpr int NORMAL_PROB = 55; // 40% обычные
    constexpr int HARDENED_PROB = 20; // 20% упрочнённые
    constexpr int BONUS_PROB = 15; // 15% с бонусами
    constexpr int SPEEDUP_PROB = 5; // 5% ускоряющие
    constexpr int INDESTRUCTIBLE_PROB = 5; // 5% неразрушаемые

    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < blocksInRow; c++) {
            std::unique_ptr<Block> block;
            int randType = RandomGenerator::getInstance().randomInRange(1, 100);
            if (randType <= NORMAL_PROB) {
                block = std::make_unique<NormalBlock>();
            } else if (randType <= NORMAL_PROB + HARDENED_PROB) {
                block = std::make_unique<HardenedBlock>();
            } else if (randType <= NORMAL_PROB + HARDENED_PROB + BONUS_PROB) {
                block = std::make_unique<BonusBlock>();
            } else if (randType <= NORMAL_PROB + HARDENED_PROB + BONUS_PROB + SPEEDUP_PROB) {
                block = std::make_unique<SpeedUpBlock>();
            } else {
                block = std::make_unique<IndestructibleBlock>();
            }
            // Даём блоку координаты
            float x = offsetX + c * (blockWidth + marginX);
            float y = offsetY + r * (blockHeight + marginY);
            block->setPosition(sf::Vector2f(x, y), sf::Vector2f(blockWidth, blockHeight));
            blocks.push_back(std::move(block));
        }
    }
}

void Arcanoid::handlePaddleInput(float dt) {
    if (isPaused || isGameOver) return;
    bool left = sf::Keyboard::isKeyPressed(sf::Keyboard::Left);
    bool right = sf::Keyboard::isKeyPressed(sf::Keyboard::Right);
    paddle.setInput(left, right);
}

void Arcanoid::handleEvent(const sf::Event& event) {
    if (event.type == sf::Event::KeyPressed) {
        switch (event.key.code) {
            case sf::Keyboard::Space:
                if (!isPaused && ball.isAttached()) {
                    ball.launch(originalBallSpeed);
                    ballStickTimer = 0.0f;
                }
                break;
            case sf::Keyboard::P:
                if (!isGameOver) switchPause();
                break;
            case sf::Keyboard::R:
                restartGame();
                break;
            default: break;
        }
    }
}

void Arcanoid::update(float dt) {
    if (isPaused || isGameOver) return;
    // Обновляем платформу
    paddle.update(dt);
    // Проверяем таймеры бонусов
    if (paddleResizeTimer > 0.0f) {
        paddleResizeTimer -= dt;
        if (paddleResizeTimer <= 0.0f) {
            paddle.resetToOriginalWidth();
        }
    }
    
    if (ballStickTimer > 0.0f) {
        ballStickTimer -= dt;
        if (ballStickTimer <= 0.0f && ball.isAttached()) {
            ball.launch(originalBallSpeed);
        }
    }

    // Если шарик не прилип, обновляем его физику
    if (!ball.isAttached()) {
        // Обновляем шарик 
        ball.update(dt);
        // Проверка столкновений со стенами
        checkCollisionWithWalls();
        // Проверка столкновения с платформой
        checkCollisionWithPaddle();
        // Здесь проверка столкновений с блоками
        checkCollisionsWithBlocks();
        // Проверка потери шарика (нижняя граница)
        checkBallLost();
        // Прооверка, что все блоки уничтожены
        if (countRemainingBlocks() == 0) {
            isGameOver = true;
            isVictory = true;
        }
    } else {
        // Если шарик прилип, обновляем его позицию за платформой
        ball.setPosition(paddle.getPosition().x + paddle.getWidth() / 2.0f, paddle.getPosition().y - ball.getRadius());
    }
    // Бонусы падают
    updateBonuses(dt);
}

void Arcanoid::draw(sf::RenderWindow& window) {
    // Отрисовка блоков
    for (auto& block : blocks) {
        if (block->isAlive()) block->draw(window);
    }
    // Отрисовка бонусов
    for (auto& bonus : bonuses) {
        if (bonus->isAlive()) bonus->draw(window);
    }
    // Отрисовка шарика и платформы
    paddle.draw(window);
    ball.draw(window);
    
    // Отрисовка счёта
    sf::Text scoreText;
    scoreText.setFont(font);
    scoreText.setCharacterSize(30);
    scoreText.setString("Score: " + std::to_string(score));
    scoreText.setFillColor(sf::Color::Black);
    scoreText.setOutlineColor(sf::Color::White);
    scoreText.setOutlineThickness(1.0f);
    scoreText.setPosition(15.0f, 15.0f);
    window.draw(scoreText);
    
    // Отрисовка активных бонусов
    int bonusLine = 0;
    if (paddleResizeTimer > 0.0f) {
        sf::Text txt;
        txt.setFont(font);
        txt.setCharacterSize(16);
        txt.setString("Paddle: " + std::to_string(static_cast<int>(paddleResizeTimer)) + "s");
        txt.setFillColor(sf::Color::White);
        txt.setPosition(width - 100.0f, 20.0f + bonusLine * 20.0f);
        window.draw(txt);
        bonusLine++;
    }
    
    if (ballStickTimer > 0.0f) {
        sf::Text txt;
        txt.setFont(font);
        txt.setCharacterSize(16);
        txt.setString("Stick: " + std::to_string(static_cast<int>(ballStickTimer)) + "s");
        txt.setFillColor(sf::Color::White);
        txt.setPosition(width - 100.0f, 20.0f + bonusLine * 20.0f);
        window.draw(txt);
        bonusLine++;
    }
    
    if (bottomShieldActive) {
        sf::Text txt;
        txt.setFont(font);
        txt.setCharacterSize(16);
        txt.setString("Shield ready");
        txt.setFillColor(sf::Color::White);
        txt.setPosition(width - 100.0f, 20.0f + bonusLine * 20.0f);
        window.draw(txt);
        
        sf::RectangleShape shield;
        shield.setPosition(0.0f, height - 10.0f);
        shield.setSize(sf::Vector2f(width, 10.0f));
        shield.setOutlineColor(sf::Color::Black);
        shield.setOutlineThickness(-1.0f);
        shield.setFillColor(sf::Color::White);
        window.draw(shield);
    }
    
    // Отрисовка экрана паузы
    if (isPaused) {
        sf::RectangleShape pauseRect(sf::Vector2f(width, height));
        pauseRect.setFillColor(sf::Color(0, 0, 0, 127));
        window.draw(pauseRect);
        
        sf::Text pauseText;
        pauseText.setFont(font);
        pauseText.setCharacterSize(40);
        pauseText.setString("Paused");
        pauseText.setFillColor(sf::Color::White);
        sf::FloatRect tb = pauseText.getLocalBounds();
        pauseText.setOrigin(tb.left + tb.width/2, tb.top + tb.height/2);
        pauseText.setPosition(width/2.0f, height/2.0f);
        window.draw(pauseText);
    }
    
    // Отрисовка экрана GameOver (и Victory)
    if (isGameOver) {
        sf::RectangleShape gameOverRect(sf::Vector2f(width, height));
        gameOverRect.setFillColor(sf::Color(0, 0, 0, 180));
        window.draw(gameOverRect);
        
        sf::Text gameOverText;
        gameOverText.setFont(font);
        gameOverText.setCharacterSize(50);
        if (isVictory) {
            gameOverText.setString("You won!");
            gameOverText.setFillColor(sf::Color::Green);
        } else {
            gameOverText.setString("Game Over");
            gameOverText.setFillColor(sf::Color::Red);
        }
        sf::FloatRect tb = gameOverText.getLocalBounds();
        gameOverText.setOrigin(tb.left + tb.width/2, tb.top + tb.height/2);
        gameOverText.setPosition(width/2.0f, height/2.0f - 50);
        window.draw(gameOverText);
        
        sf::Text restartText;
        restartText.setFont(font);
        restartText.setCharacterSize(30);
        restartText.setString("Press R to restart");
        restartText.setFillColor(sf::Color::White);
        tb = restartText.getLocalBounds();
        restartText.setOrigin(tb.left + tb.width/2, tb.top + tb.height/2);
        restartText.setPosition(width/2.0f, height/2.0f + 50);
        window.draw(restartText);
    }
}

void Arcanoid::checkCollisionWithWalls() {
    sf::FloatRect ballRect = ball.getRect();
    // Левая и правыя стена
    if (ballRect.left < 0) {
        ball.setPosition(ball.getRadius(), ballRect.top + ball.getRadius());
        ball.reflectX();
    }
    if (ballRect.left + ballRect.width > width) {
        ball.setPosition(width - ball.getRadius(), ballRect.top + ball.getRadius());
        ball.reflectX();
    }
    // Верхняя стена
    if (ballRect.top < 0) {
        ball.setPosition(ballRect.left + ball.getRadius(), ball.getRadius());
        ball.reflectY();
    }
}

void Arcanoid::checkCollisionWithPaddle() {
    sf::FloatRect ballRect = ball.getRect();
    sf::FloatRect paddleRect = paddle.getRect();
    if (ballRect.intersects(paddleRect)) {
        ball.bounceFromPaddle(paddle);
    }
}

void Arcanoid::checkBallLost() {
    if (ball.getRect().top + ball.getRect().height > height) {
        // Проверяем присутствие щита
        if (bottomShieldActive) {
            bottomShieldActive = false;
            resetBall();
        } else {
            score -= 10;
            resetBall();
            // Если количество баллов слишком мало - GameOver
            if (score < -30) {
                isGameOver = true;
            }
        }
    }
}

void Arcanoid::checkCollisionsWithBlocks() {
    sf::FloatRect ballRect = ball.getRect();
    for (auto& block : blocks) {
        if (!block->isAlive()) continue;

        sf::FloatRect blockRect = block->getRect();
        // Проверка столкновения через boundingBox
        if (ballRect.intersects(blockRect)) {
            float blockRight = blockRect.left + blockRect.width;
            float blockBottom = blockRect.top + blockRect.height;
            float ballRight = ballRect.left + ballRect.width;
            float ballBottom = ballRect.top + ballRect.height;
            
            float leftOverlap = ballRight - blockRect.left;
            float rightOverlap = blockRight - ballRect.left;
            float topOverlap = ballBottom - blockRect.top;
            float bottomOverlap = blockBottom - ballRect.top;
            // Минимальное покрытие - сторона удара
            float minOverlap = std::min({leftOverlap, rightOverlap, topOverlap, bottomOverlap});
            
            if (minOverlap == leftOverlap || minOverlap == rightOverlap) {
                ball.reflectX();
                if (minOverlap == leftOverlap) {
                    ball.setPosition(blockRect.left - ball.getRadius(), ballRect.top + ball.getRadius());
                } else {
                    ball.setPosition(blockRight + ball.getRadius(), ballRect.top + ball.getRadius());
                }
            } else {
                ball.reflectY();
                if (minOverlap == topOverlap) {
                    ball.setPosition(ballRect.left + ball.getRadius(), blockRect.top - ball.getRadius());
                } else {
                    ball.setPosition(ballRect.left + ball.getRadius(), blockBottom + ball.getRadius());
                }
            }
            
            block->onHit(ball);
            score += block->getScoreValue();
            // Если блок уничтожен
            if (!block->isAlive()) {
                block->onDestroy(*this);   // полиморфный вызов
            }
            break;
        }
    }
}

void Arcanoid::spawnBonus(const sf::Vector2f& pos, int bonusType) {
    std::unique_ptr<Bonus> bonus;
    switch (bonusType) {
        case 0: {
            bonus = std::make_unique<ExpandPaddleBonus>();
            break;
        }
        case 1: {
            bonus = std::make_unique<ShrinkPaddleBonus>();
            break;
        }
        case 2: {
            bonus = std::make_unique<SpeedUpBallBonus>();
            break;
        }
        case 3: {
            bonus = std::make_unique<SlowDownBallBonus>();
            break;
        }
        case 4: {
            bonus = std::make_unique<StickBonus>();
            break;
        }
        case 5: {
            bonus = std::make_unique<BottomShieldBonus>();
            break;
        }
        case 6: {
            bonus = std::make_unique<RandomTurnBonus>();
            break;
        }
        default: break;
    }
    bonus->spawn(pos);
    bonus->setScreenHeight(height); // Сохраняем высоту экрана
    bonuses.push_back(std::move(bonus));
}

void Arcanoid::updateBonuses(float dt) {
    // Цикл по вектору (через iterator)
    for (auto it = bonuses.begin(); it != bonuses.end(); ) {
        (*it)->update(dt);
        
        if (!(*it)->isAlive()) {
            it = bonuses.erase(it);
            continue;
        }
        
        if ((*it)->getRect().intersects(paddle.getRect())) {
            (*it)->apply(*this);
            it = bonuses.erase(it);
        } else {
            it++;
        }
    }
}

void Arcanoid::resetBall() {
    ball.resetToPaddle(paddle);
    ballStickTimer = 0.0f;
}

void Arcanoid::switchPause() {
    isPaused = !isPaused;
}

void Arcanoid::restartGame() {
    score = 0;
    isPaused = false;
    isGameOver = false;
    isVictory = false;
    bottomShieldActive = false;
    paddleResizeTimer = 0.0f;
    ballStickTimer = 0.0f;
    
    paddle.resetToOriginalWidth();
    paddle.setScreenWidth(width);
    paddle.init((width - 100.0f) / 2.0f, height - 50.0f, 100.0f, 10.0f, 400.0f);
    
    resetBall();
    initializeBlocks();
    resetBonusEffects();
}

int Arcanoid::countRemainingBlocks() const {
    int count = 0;
    for (const auto& block : blocks) {
        if (block->isAlive() && block->isDestructible()) {
            count++;
        }
    }
    return count;
}

void Arcanoid::resetBonusEffects() {
    bonuses.clear();
    paddleResizeTimer = 0.0f;
    ballStickTimer = 0.0f;
    bottomShieldActive = false;
    paddle.resetToOriginalWidth();
}

// Реализуем методы бонусов
void Arcanoid::expandPaddle() {
    paddle.setWidth(paddle.getWidth() * 1.4f);
    paddleResizeTimer = 10.0f;
}

void Arcanoid::shrinkPaddle() {
    paddle.setWidth(paddle.getWidth() * 0.6f);
    paddleResizeTimer = 10.0f;
}

void Arcanoid::speedUpBall() {
    if (!ball.isAttached()) {
        sf::Vector2f vel = ball.getVelocity();
        float speed = std::hypot(vel.x, vel.y);
        speed = std::min(speed * 1.3f, 800.0f);
        ball.setVelocity(vel.x * speed / std::hypot(vel.x, vel.y), vel.y * speed / std::hypot(vel.x, vel.y));
    }
}

void Arcanoid::slowDownBall() {
    if (!ball.isAttached()) {
        sf::Vector2f vel = ball.getVelocity();
        float speed = std::hypot(vel.x, vel.y);
        speed = std::max(speed * 0.77f, 200.0f);
        ball.setVelocity(vel.x * speed / std::hypot(vel.x, vel.y), vel.y * speed / std::hypot(vel.x, vel.y));
    }
}

void Arcanoid::stickBall() {
    ball.startStickTimer(5.0f);
    ballStickTimer = 5.0f;
}

void Arcanoid::activateBottomShield() {
    bottomShieldActive = true;
}

void Arcanoid::randomTurn() {
    if (!ball.isAttached()) {
        sf::Vector2f vel = ball.getVelocity();
        float speed = std::hypot(vel.x, vel.y);
        int angleDeg = RandomGenerator::getInstance().randomInRange(-70, 70);
        float angle = angleDeg * 3.14159265358979323846f / 180.0f;
        float directionY = (vel.y > 0.0f) ? 1.0f : -1.0f;
        ball.setVelocity(speed * std::sin(angle), directionY * speed * std::cos(angle));
    }
}

Arcanoid::~Arcanoid() = default;
