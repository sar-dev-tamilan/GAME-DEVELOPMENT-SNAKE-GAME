#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <ctime>

using namespace sf;

const int WIDTH = 800;
const int HEIGHT = 600;
const int GRID_SIZE = 20;

struct SnakeSegment {
    int x, y;
};

class SnakeGame {
private:
    RenderWindow window;
    std::vector<SnakeSegment> snake;
    Vector2i food;
    Vector2i direction;
    Clock clock;
    float speed;
    bool gameOver;
    Font font;
    Text gameOverText;
    SoundBuffer eatBuffer, gameOverBuffer;
    Sound eatSound, gameOverSound;
    
public:
    SnakeGame() : window(VideoMode(WIDTH, HEIGHT), "Snake Game"), speed(0.1f), gameOver(false), direction(1, 0) {
        snake.push_back({WIDTH / GRID_SIZE / 2, HEIGHT / GRID_SIZE / 2});
        spawnFood();
        
        font.loadFromFile("arial.ttf");
        gameOverText.setFont(font);
        gameOverText.setCharacterSize(40);
        gameOverText.setFillColor(Color::Red);
        gameOverText.setPosition(WIDTH / 4, HEIGHT / 2);
        gameOverText.setString("Game Over! Press R to Restart");
        
        eatBuffer.loadFromFile("eat.wav");
        eatSound.setBuffer(eatBuffer);
        gameOverBuffer.loadFromFile("gameover.wav");
        gameOverSound.setBuffer(gameOverBuffer);
    }

    void spawnFood() {
        food.x = rand() % (WIDTH / GRID_SIZE);
        food.y = rand() % (HEIGHT / GRID_SIZE);
    }
    
    void handleInput() {
        if (Keyboard::isKeyPressed(Keyboard::Up) && direction.y == 0) direction = {0, -1};
        if (Keyboard::isKeyPressed(Keyboard::Down) && direction.y == 0) direction = {0, 1};
        if (Keyboard::isKeyPressed(Keyboard::Left) && direction.x == 0) direction = {-1, 0};
        if (Keyboard::isKeyPressed(Keyboard::Right) && direction.x == 0) direction = {1, 0};
        if (Keyboard::isKeyPressed(Keyboard::R)) restart();
    }
    
    void update() {
        if (gameOver) return;
        if (clock.getElapsedTime().asSeconds() < speed) return;
        clock.restart();
        
        SnakeSegment newHead = {snake[0].x + direction.x, snake[0].y + direction.y};
        
        if (newHead.x < 0 || newHead.x >= WIDTH / GRID_SIZE || newHead.y < 0 || newHead.y >= HEIGHT / GRID_SIZE) {
            gameOver = true;
            gameOverSound.play();
            return;
        }
        
        for (const auto& segment : snake) {
            if (newHead.x == segment.x && newHead.y == segment.y) {
                gameOver = true;
                gameOverSound.play();
                return;
            }
        }
        
        snake.insert(snake.begin(), newHead);
        if (newHead.x == food.x && newHead.y == food.y) {
            eatSound.play();
            spawnFood();
            speed *= 0.95;
        } else {
            snake.pop_back();
        }
    }
    
    void draw() {
        window.clear(Color::Black);
        
        RectangleShape foodShape(Vector2f(GRID_SIZE, GRID_SIZE));
        foodShape.setFillColor(Color::Red);
        foodShape.setPosition(food.x * GRID_SIZE, food.y * GRID_SIZE);
        window.draw(foodShape);
        
        RectangleShape snakeShape(Vector2f(GRID_SIZE, GRID_SIZE));
        for (const auto& segment : snake) {
            snakeShape.setPosition(segment.x * GRID_SIZE, segment.y * GRID_SIZE);
            snakeShape.setFillColor(Color::Green);
            window.draw(snakeShape);
        }
        
        if (gameOver) window.draw(gameOverText);
        
        window.display();
    }
    
    void restart() {
        gameOver = false;
        snake.clear();
        snake.push_back({WIDTH / GRID_SIZE / 2, HEIGHT / GRID_SIZE / 2});
        direction = {1, 0};
        speed = 0.1f;
        spawnFood();
    }
    
    void run() {
        while (window.isOpen()) {
            Event event;
            while (window.pollEvent(event)) {
                if (event.type == Event::Closed) window.close();
            }
            
            handleInput();
            update();
            draw();
        }
    }
};

int main() {
    srand(time(0));
    SnakeGame game;
    game.run();
    return 0;
}