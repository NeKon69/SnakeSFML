#include <SFML/Graphics.hpp>
#include <random>
#include <complex>
#include <vector>
#include <thread>
#include <optional>
#include <iostream>
#include <vector_alias.hpp>

class SnakeGame : public sf::Drawable {
private:
    struct SnakeSegment {
        sf::Vector2f coords;
        bool head = false;
    };

    raw::vector<SnakeSegment> snakeData;
    sf::Vector2f prevCoords;
    sf::Vector2f foodCoords;

    sf::Keyboard::Scancode prevMove = sf::Keyboard::Scancode::W;
    sf::Keyboard::Scancode currMove = sf::Keyboard::Scancode::W;

    std::random_device rd;
    std::mt19937 gen;
    std::uniform_int_distribution<> xDist;
    std::uniform_int_distribution<> yDist;

public:
    SnakeGame() : gen(rd()), xDist(0, 79), yDist(0, 59) {
        snakeData.reserve(50000);
        snakeData.push_back({ {100, 100}, true });
        generateApple();
    }

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
        for (const auto& segment : snakeData) {
            sf::RectangleShape rect({ 10, 10 });
            rect.setFillColor(segment.head ? sf::Color::Red : sf::Color::Green);
            rect.setPosition(segment.coords);
            target.draw(rect);
        }

        sf::CircleShape food(5);
        food.setPosition(foodCoords);
        target.draw(food);
    }

    void move(const sf::Keyboard::Scancode& button) {
        currMove = button;
    }
    void move(const sf::Event::KeyPressed& button) {
        currMove = button.scancode;
    }



    void move() {
        prevCoords = snakeData[0].coords;
        if (currMove == sf::Keyboard::Scancode::A) {
            if (prevMove != sf::Keyboard::Scancode::D)
            { snakeData[0].coords.x -= 10; prevMove = currMove; }
            else
                snakeData[0].coords.x += 10;
        }

        if (currMove == sf::Keyboard::Scancode::D) {
            if (prevMove != sf::Keyboard::Scancode::A)
            { snakeData[0].coords.x += 10; prevMove = currMove; }
            else
                snakeData[0].coords.x -= 10;
        }
        if (currMove == sf::Keyboard::Scancode::W) {
            if (prevMove != sf::Keyboard::Scancode::S)
            { snakeData[0].coords.y -= 10; prevMove = currMove; }
            else
                snakeData[0].coords.y += 10;
        }
        if (currMove == sf::Keyboard::Scancode::S) {
            if (prevMove != sf::Keyboard::Scancode::W)
            { snakeData[0].coords.y += 10; prevMove = currMove; }
            else
                snakeData[0].coords.y -= 10;
        }

        for (int i = 1; i < snakeData.get_size(); ++i) {
            if (i == 1)
                snakeData[1].coords = prevCoords;
            else
                snakeData[i].coords = snakeData[i + 1].coords;
        }

        prevCoords = snakeData[0].coords;
        std::cout << "------------------COORDS------------------" << "\n";
        for (int i = 0; i < snakeData.get_size(); ++i) {
            std::cout << i + 1 << ": " << snakeData[i].coords.x << "    " << snakeData[i].coords.y << std::endl;
        }
        std::cout << "prev_coords" << ": " << prevCoords.x << "    " << prevCoords.y << "\n";
    }

    void add_snake() {
        sf::Vector2f lastCoords = snakeData.get_size() == 1 ? prevCoords : snakeData[snakeData.get_size() - 1].coords;
        sf::Vector2f secondLastCoords = snakeData[snakeData.get_size() - 2].coords;
        sf::Vector2f newCoords = lastCoords;

        SnakeSegment snk;
        snk.coords = newCoords;
        snakeData.push_back(snk);
    }

    void generateApple() {
        bool valid = false;
        while (!valid) {
            foodCoords = { float(xDist(gen) * 10), float(yDist(gen) * 10) };
            valid = true;
            for (const auto& segment : snakeData) {
                if (segment.coords == foodCoords) {
                    valid = false;
                    break;
                }
            }
        }
    }

    void update(sf::Time deltaTime) {
        static sf::Time elapsedTime;
        elapsedTime += deltaTime;

        if (elapsedTime.asMilliseconds() > 100) {
            if(checkCollision())
				std::cout << "Game over!" << std::endl;
            move();
            if (snakeData[0].coords == foodCoords) {
                add_snake();
                generateApple();
            }
            elapsedTime = sf::Time::Zero;
        }
    }

    bool checkCollision() {
        if (snakeData[0].coords.x < 0 || snakeData[0].coords.x >= 800 ||
            snakeData[0].coords.y < 0 || snakeData[0].coords.y >= 600) {
            return true;
        }

        for (size_t i = 1; i < snakeData.get_size(); ++i) {
            if (snakeData[0].coords == snakeData[i].coords) {
                return true;
            }
        }

        return false;
    }
};

int main() {
	sf::RenderWindow window(sf::VideoMode({ 800, 600 }), "Snake game");
	SnakeGame game;
	sf::Clock clock;
	while (window.isOpen()) {
		while (const auto event = window.pollEvent()) {
			if (event->is<sf::Event::Closed>()) {
				window.close();
			}
			if (const auto* button = event->getIf<sf::Event::KeyPressed>()) {
				if (button->scancode == sf::Keyboard::Scancode::W || button->scancode == sf::Keyboard::Scancode::S || button->scancode == sf::Keyboard::Scancode::A || button->scancode == sf::Keyboard::Scancode::D)
					game.move(*button);
				else if (button->scancode == sf::Keyboard::Scancode::Escape) {
					window.close();
				}
			}
		}
		game.update(clock.restart());
		window.clear();
		window.draw(game);
		window.display();
	}

}