#include <iostream>
#include <cmath>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <algorithm>
#include <random>
#include <ctime>
#include <vector>

// Типы блоков
enum class BlockType {
    Normal, // Обычный разрушаемый (1 удар)
    Hardened, // Hardened (несколько ударов)
    Bonus, // С бонусом (при разрушении)
    SpeedUp, // Увеличивает скорость мяча
    Indestructible // Неразрушаемый
};

// Класс блока
struct Block {
    BlockType type;
    int health; // Количество оставшихся ударов (для Hardened)
    sf::Vector2f pos; // Позиция левого верхнего угла
    sf::Vector2f size; // Размер блока
    bool isAlive; // Активен ли блок
    Block() : type(BlockType::Normal), health(1), isAlive(true) {}
};


class Arkanoid {
public:
    Arkanoid(int w, int h): width(w), height(h), score(0), isPaused(false), isGameOver(false){
        // Инициализация генератора случайных чисел
        rng.seed(static_cast<unsigned int>(std::time(nullptr)));
        // Загружаем шрифт
        if (!font.loadFromFile(PATH_TO_DATA "arial.ttf")) {
            std::cout << "Warning: Could not load font" << std::endl;
        }
        
        // Инициализируем платформу (её левый верхний угол)
        paddle.width = 100.f;
        paddle.height = 10.0f;
        paddle.pos.x = (width - paddle.width) / 2.0f;
        paddle.pos.y = height - 50.0f;
        paddle.speed = 400.0f; // Скорость платфомы

        // Инициализируем шарик
        ball.radius = 10.0f;
        ball.attached = true; // Начальное состояние - прилип к платформе
        ball.pos.x = paddle.pos.x + paddle.width / 2.0f;
        ball.pos.y = paddle.pos.y - ball.radius;
        ball.velocity.x = 0.0f;
        ball.velocity.y = 0.0f;
        // Инициализируем блоки
        initializeBlocks();
    }

    // Инициализация сетки блоков
    void initializeBlocks() {
        blocks.clear();
        // Параметры сетки блоков
        const int blocksInRow = 10; // Кол-во блоков в ряду
        const int rows = 5; // Кол-во рядов
        const float offsetX = 50.0f; // Расстояние от границ до блоков
        const float offsetY = 50.0f; // Отступ сверху до блоков
        const float marginX = 5.0f; // Расстояние между блоками (x)
        const float marginY = 5.0f; // Растояние между блоками (y)
        const float blockHeight = 30.0f; // Высота блока
        float totalWidth = width - 2.0f * offsetX; // Общая ширина таблицы блоков
        float blockWidth = (totalWidth - marginX * static_cast<float>(blocksInRow - 1)) / static_cast<float>(blocksInRow); // Щирина блока
        // Вероятности появление разных типов блоков (в процентах) (сумма - 100%)
        const int NORMAL_PROB = 55; // 40% обычные
        const int HARDENED_PROB = 20; // 20% упрочнённые
        const int BONUS_PROB = 15; // 15% с бонусами
        const int SPEEDUP_PROB = 5; // 5% ускоряющие
        const int INDESTRUCTIBLE_PROB = 5; // 5% неразрушаемые
        
        for (int r = 0; r < rows; r++) {
            for (int c = 0; c < blocksInRow; c++) {
                Block block;
                block.size = sf::Vector2f(blockWidth, blockHeight);
                block.pos.x = offsetX + c * (blockWidth + marginX);
                block.pos.y = offsetY + r * (blockHeight + marginY);
                block.isAlive = true;
                int randType = randomInRange(1, 100);
                // Выбираем тип блока
                if (randType <= NORMAL_PROB) {
                    block.type = BlockType::Normal;
                    block.health = 1;
                } 
                else if (randType <= NORMAL_PROB + HARDENED_PROB) {
                    block.type = BlockType::Hardened;
                    block.health = randomInRange(2, 4); // 2-4 удара
                }
                else if (randType <= NORMAL_PROB + HARDENED_PROB + BONUS_PROB) {
                    block.type = BlockType::Bonus;
                    block.health = 1;
                }
                else if (randType <= NORMAL_PROB + HARDENED_PROB + BONUS_PROB + SPEEDUP_PROB) {
                    block.type = BlockType::SpeedUp;
                    block.health = 1;
                }
                else {
                    block.type = BlockType::Indestructible;
                    block.health = -1; // Бесконечное здоровье
                }
                blocks.push_back(block);
            }
        }
    }
    // Обработка нажатий клавиш движения платформы (для непрерывного движения)
    void handlePaddleInput(float dt) {
        if (isPaused || isGameOver) return;
        
        // Движение платформы влево-вправо
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            paddle.pos.x -= paddle.speed * dt;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            paddle.pos.x += paddle.speed * dt;
        }
        // Ограничиваем платформу границами экрана
        paddle.pos.x = std::clamp(paddle.pos.x, 0.0f, width - paddle.width);
        
        // Если шарик прилип, обновляем его позицию за платформой
        if (ball.attached) {
            ball.pos.x = paddle.pos.x + paddle.width / 2.0f;
            ball.pos.y = paddle.pos.y - ball.radius;
        }        
    }

    // Обработка событий (нажатия клавиш)
    void handleEvent(const sf::Event& event) {
        if (event.type == sf::Event::KeyPressed){
            // Проверяем установленные клавиши
            switch(event.key.code) {
                case sf::Keyboard::Space:
                    if (!isPaused && ball.attached) {
                        // Здесь запустить шарик
                        launchBall();
                    }
                    break;

                case sf::Keyboard::P:
                    if (!isGameOver) {
                        // Переключить паузу
                        switchPause();
                    }
                    break;
                    
                case sf::Keyboard::R:
                    // Перезапусть игру
                    restartGame();
                    break;
                    
                default:
                    break;
                    
            }
        }
    }

    void draw(sf::RenderWindow& window) {
        // Отрисовка блоков
        for (const Block& block: blocks) {
            if (!block.isAlive) continue;

            sf::RectangleShape blockRect(block.size);
            blockRect.setPosition(block.pos);
            blockRect.setOutlineColor(sf::Color::Black);
            blockRect.setOutlineThickness(-2.0f);
            
            // Текст на блоке
            sf::Text blockText;
            blockText.setFont(font);
            blockText.setCharacterSize(static_cast<unsigned int>(block.size.y * 0.6f));
            // Олтичия для каждого типа блока
            float blockDark = 1.0f - (block.health - 1) * 0.1f; // Для hardened
            switch(block.type) {
                case BlockType::Normal:
                    blockRect.setFillColor(sf::Color(124, 252, 0));
                    blockText.setString("");
                    break;
                case BlockType::Hardened:
                    //int alpha = 100 + (block.health - 1) * 30;
                    
                    // Чем больше здоровья, тем толще граница
                    blockRect.setFillColor(sf::Color(124 * blockDark, 252 * blockDark, 0));
                    blockRect.setOutlineThickness(blockRect.getOutlineThickness()*block.health);
                    blockText.setString(std::to_string(block.health));
                    break;
                case BlockType::Bonus:
                    blockRect.setFillColor(sf::Color(255, 215, 0));
                    blockText.setString("?");
                    break;
                case BlockType::SpeedUp:
                    blockRect.setFillColor(sf::Color(0, 191, 255));
                    blockText.setString(">>");
                    break;
                case BlockType::Indestructible:
                    blockRect.setFillColor(sf::Color(128, 128, 128)); //Серый
                    blockText.setString("#");
                    break;
            }
            blockText.setFillColor(sf::Color::Black);
            // Выравниваем текст
            sf::FloatRect textBounds = blockText.getLocalBounds();
            blockText.setOrigin(textBounds.left + textBounds.width / 2.0f, textBounds.top + textBounds.height / 2.0f);
            blockText.setPosition(block.pos.x + block.size.x / 2.0f, block.pos.y + block.size.y / 2.0f);  

            window.draw(blockRect);
            window.draw(blockText);
        }
        // Отрисовка платформы
        sf::RectangleShape paddleRect(sf::Vector2f(paddle.width, paddle.height));
        paddleRect.setPosition(paddle.pos);
        paddleRect.setFillColor(sf::Color::White);
        paddleRect.setOutlineColor(sf::Color::Black);
        paddleRect.setOutlineThickness(-3.0f);
        window.draw(paddleRect);

        // Отрисовка шарика
        sf::CircleShape ballShape(ball.radius);
        ballShape.setPosition(ball.pos.x - ball.radius, ball.pos.y - ball.radius);
        ballShape.setFillColor(sf::Color::White);
        ballShape.setOutlineColor(sf::Color::Black);
        ballShape.setOutlineThickness(-2.0f);
        window.draw(ballShape);

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

        // Отрисовка экрана паузы
        if (isPaused) {
            sf::RectangleShape pauseRect(sf::Vector2f(width, height));
            pauseRect.setPosition(0.0f, 0.0f);
            pauseRect.setFillColor(sf::Color(0, 0, 0, 127));
            window.draw(pauseRect);

            sf::Text pauseText;
            pauseText.setFont(font);
            pauseText.setCharacterSize(40);
            pauseText.setString("Paused");
            pauseText.setFillColor(sf::Color::White);
            sf::FloatRect textBounds = pauseText.getLocalBounds();
            // Ставим origin по центру boundingBox
            pauseText.setOrigin(textBounds.left + textBounds.width/2, textBounds.top + textBounds.height/2);
            // Устанавливаем по центру
            pauseText.setPosition(width/2.0f, height/2.0f);
            window.draw(pauseText);
        }

        // Отрисовка экрана GameOver
        if (isGameOver) {
            sf::RectangleShape gameOverRect(sf::Vector2f(width, height));
            gameOverRect.setFillColor(sf::Color(0, 0, 0, 180));
            window.draw(gameOverRect);
            
            sf::Text gameOverText;
            gameOverText.setFont(font);
            gameOverText.setCharacterSize(50);
            gameOverText.setString("Game Over");
            gameOverText.setFillColor(sf::Color::Red);
            sf::FloatRect textBounds = gameOverText.getLocalBounds();
            gameOverText.setOrigin(textBounds.left + textBounds.width / 2.0f, textBounds.top + textBounds.height / 2.0f);
            gameOverText.setPosition(width / 2.0f, height / 2.0f - 50);
            window.draw(gameOverText);
            
            sf::Text restartText;
            restartText.setFont(font);
            restartText.setCharacterSize(30);
            restartText.setString("Press R to restart");
            restartText.setFillColor(sf::Color::White);
            textBounds = restartText.getLocalBounds();
            restartText.setOrigin(textBounds.left + textBounds.width / 2.0f, textBounds.top + textBounds.height / 2.0f);
            restartText.setPosition(width / 2.0f, height / 2.0f + 50);
            window.draw(restartText);
        }
    }

    void update(float dt) {
        if (isPaused || isGameOver) return;

        // На всякий ограничим положение платформы
        paddle.pos.x = std::clamp(paddle.pos.x, 0.0f, width - paddle.width);

        // Если шарик не прилип, обновляем его физику
        if (!ball.attached) {
            // Движение шарика
            ball.pos.x += ball.velocity.x * dt;
            ball.pos.y += ball.velocity.y * dt;
            
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
            }
        }        
    }
private:
    int width, height;
    int score;
    bool isPaused;
    bool isGameOver;
    sf::Font font;
    std::mt19937 rng;
    
    struct Paddle {
        sf::Vector2f pos; // Позиция левого угла платформы
        float width;
        float height;
        float speed;
    } paddle;

    struct Ball {
        sf::Vector2f pos; // Позиция цента шарика
        sf::Vector2f velocity;
        float radius;
        bool attached;  // Прилип к платформе
    } ball;

    std::vector<Block> blocks; // Список блоков
    
    // Генератор рандомных чисел в диапазоне
    int randomInRange(int a, int b){
        std::uniform_int_distribution<int> dist(a, b);
        return dist(rng);
    }

    int countRemainingBlocks() {
        int count = 0;
        for (const Block& block : blocks) {
            if (block.isAlive && block.type != BlockType::Indestructible) {
                count++;
            }
        }
        return count;
    }

    // Запускаем шарик с фикс. скоростью вверх
    void launchBall() {
        ball.attached = false;
        ball.velocity.x = 0.0f;
        ball.velocity.y = -400.0f;
    }

    // Взвращаем шарик на платфору
    void resetBall() {
        ball.attached = true;
        ball.pos.x = paddle.pos.x + paddle.width / 2.0f;
        ball.pos.y = paddle.pos.y - ball.radius;
        ball.velocity.x = 0.0f;
        ball.velocity.y = 0.0f;        
    }

    // Проверяем коллизию со стенками, от которых отскакиваем
    void checkCollisionWithWalls() {
        // Левая и правая стены
        if (ball.pos.x - ball.radius < 0) {
            ball.pos.x = ball.radius;
            ball.velocity.x = -ball.velocity.x;
        }

        if (ball.pos.x + ball.radius > width) {
            ball.pos.x = width - ball.radius;
            ball.velocity.x = -ball.velocity.x;
        }
        
        // Верхняя стена
        if (ball.pos.y - ball.radius < 0) {
            ball.pos.y = ball.radius;
            ball.velocity.y = -ball.velocity.y;
        }         
    }

    // Проверка столкновения с блоками
    void checkCollisionsWithBlocks() {
        sf::FloatRect ballRect(ball.pos.x - ball.radius, ball.pos.y - ball.radius, ball.radius * 2, ball.radius * 2);

        for (Block& block : blocks) {
            // Пропускаем мёртые блоки
            if (!block.isAlive) continue;
            sf::FloatRect blockRect(block.pos.x, block.pos.y, block.size.x, block.size.y);
            // Проверка столкновения через boundingBox
            if (ballRect.intersects(blockRect)) {
                // Коллизию обрабатываем как колизию двух прямоугольников
                // Проверим, с какой стороны произошло столкновение
                float blockRect_right = blockRect.left + blockRect.width;
                float blockRect_bottom = blockRect.top + blockRect.height;
                float ballRect_right = ballRect.left + ballRect.width;
                float ballRect_bottom = ballRect.top + ballRect.height;
                float leftOverlap = ballRect_right - blockRect.left;
                float rightOverlap = blockRect_right - ballRect.left;
                float topOverlap = ballRect_bottom - blockRect.top;
                float bottomOverlap = blockRect_bottom - ballRect.top;
                // Минимальное покрытие - сторона удара
                float minOverlap = std::min({leftOverlap, rightOverlap, topOverlap, bottomOverlap});
                if (minOverlap == leftOverlap) {
                    ball.velocity.x = -ball.velocity.x;
                    ball.pos.x = blockRect.left - ball.radius;
                } else if (minOverlap == rightOverlap){
                    ball.velocity.x = -ball.velocity.x;
                    ball.pos.x = blockRect_right + ball.radius;
                } else if (minOverlap == topOverlap) {
                    ball.velocity.y = -ball.velocity.y;
                    ball.pos.y = blockRect.top - ball.radius;
                } else {
                    ball.velocity.y = -ball.velocity.y;
                    ball.pos.y = blockRect_bottom + ball.radius;
                }
                // Обработка попадания по блоку
                handleBlockHit(block);
                // Выходим из цикла т.к. обрабатываем только одно столкновение за кадр
                break;
            }
        }
    }

    void handleBlockHit(Block& block) {
        // Неразрушаемые блоки только отскакивают
        if (block.type == BlockType::Indestructible) {
            return;
        }
        // Уменьшаем здоровье
        block.health--;
        // Начисляем очки за попадание
        score += 1;
        // Если блок уничтожен
        if (block.health <= 0) {
            block.isAlive = false;

            // Эффекты при уничтожении
            switch(block.type) {
                case BlockType::Bonus:
                    // Здесь надо добавить обработку бонуса
                    score += 5;
                    break;
                case BlockType::SpeedUp:
                    // Увеличиваем скорость шарика;
                    // Нужно её как-то ограничить (разумно, чтоб за кажр не пролетал через блок)
                    float maxSpeed = 800.0f;
                    float koef = 1.1f;
                    float currentSpeed = std::hypotf(ball.velocity.x, ball.velocity.y);
                    if (currentSpeed * koef <= maxSpeed) {
                        // Увеличиваем скорость, если она не будет превышать максимальную
                        ball.velocity *= koef;
                    } 
                    
                    score += 7;
                    break;
            }
        }
    }

    void checkCollisionWithPaddle() {
        // Создаём bounding box платформы и шарика
        sf::FloatRect paddleRect(paddle.pos.x, paddle.pos.y, paddle.width, paddle.height);
        sf::FloatRect ballRect(ball.pos.x - ball.radius, ball.pos.y - ball.radius, ball.radius * 2, ball.radius * 2);

        // Проверяем пересечение прямоугольников
        if (paddleRect.intersects(ballRect)) {
            // Проверяме, что шарик летит вниз
            if (ball.velocity.y > 0) {
                ball.pos.y = paddle.pos.y - ball.radius;

                // Вычислим точку касания шарика с платформой
                float paddleCenterX = paddle.pos.x + paddle.width/2.0f;
                float offsetX = ball.pos.x - paddleCenterX;
                float offset = offsetX / (paddle.width / 2.0f); // Отношение точки положения к краю платформы
                offset = std::clamp(offset, -1.0f, 1.0f); // Ограничиваем offset
                float angle = 1.22173f * offset; // 1.22173f радиан - 70 градусов
                float currentSpeed = std::hypotf(ball.velocity.x, ball.velocity.y);
                // Новая скорость
                ball.velocity.x = currentSpeed*std::sin(angle);
                ball.velocity.y = -currentSpeed*std::cos(angle);
            }
        }
    }

    void checkBallLost() {
        // Если шарик улетел за нижнюю границу
        if (ball.pos.y + ball.radius > height) {
            // Вычитаем очки
            score -= 10;           
            // Возвращаем шарик на платформу
            resetBall();
            // Если количество баллов слишком мало - GameOver
            if (score < -50) {
                isGameOver = true;
            }
        }
    }

    // Смена состояния паузы
    void switchPause() {
        isPaused = !isPaused;
    }
    
    // Сброс всех параметров игры
    void restartGame() {
        score = 0;
        isPaused = false;
        isGameOver = false;
        // Сброс платформы
        paddle.pos.x = (width - paddle.width) / 2.0f;
        // Сброс шарика
        ball.attached = true;
        ball.pos.x = paddle.pos.x + paddle.width / 2.0f;
        ball.pos.y = paddle.pos.y - ball.radius;
        ball.velocity.x = 0.0f;
        ball.velocity.y = 0.0f;
        initializeBlocks();
    }
};

int main() {
    const int WINDOW_WIDTH = 800;
    const int WINDOW_HEIGHT = 600;
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Arcanoid");
    window.setFramerateLimit(60);

    Arkanoid game(WINDOW_WIDTH, WINDOW_HEIGHT);

    sf::Clock clock;
    
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            game.handleEvent(event);
        }
        
        // Получаем время между кадрами
        float deltaTime = clock.restart().asSeconds();
        if (deltaTime > 0.02f) deltaTime = 0.02f; // Ограничиваем максимальный dt
        // Обновляем игру
        game.handlePaddleInput(deltaTime);  // обработка непрерывного ввода для платформы
        game.update(deltaTime);
        
        // Отрисовка
        window.clear(sf::Color(60, 60, 60));
        game.draw(window);
        window.display();
    }

    return 0;
}