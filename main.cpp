#include <iostream>
#include <cmath>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <algorithm>
#include <random>
#include <ctime>
#include <vector>

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
            // Проверка потери шарика (нижняя граница)
            checkBallLost();
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
    
    // Генератор рандомных чисел в диапазоне
    int randomInRange(int a, int b){
        std::uniform_int_distribution<int> dist(a, b);
        return dist(rng);
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
                float currentSpeed = std::sqrt(ball.velocity.x*ball.velocity.x + ball.velocity.y*ball.velocity.y);
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

        // Обновляем игру
        game.handlePaddleInput(deltaTime);  // обработка непрерывного ввода для платформы
        game.update(deltaTime);
        
        // Отрисовка
        window.clear(sf::Color(70, 70, 70));
        game.draw(window);
        window.display();
    }

    return 0;
}