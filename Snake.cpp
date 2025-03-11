#include <SFML/Graphics.hpp>
#include <random>
#include <complex>
#include <vector>
#include <thread>
#include <optional>
#include <iostream>
#include <vector_alias.hpp>

class SnakeGame : public sf::Drawable, public sf::Transformable {
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

    unsigned int score = 0;

public:
    SnakeGame() : gen(rd()), xDist(0, 79), yDist(0, 59) {
        snakeData.reserve(50000);
        snakeData.push_back({ {100, 100}, true });
        generateApple();
    }

    unsigned int getScore() {
        return score;
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

        for (int i = snakeData.get_size() - 1; i > 0; --i) {
            if (i == 1)
                snakeData[1].coords = prevCoords;
            else
                snakeData[i].coords = snakeData[i - 1].coords;
        }
    }

    void add_snake() {
        sf::Vector2f lastCoords = snakeData.get_size() == 1 ? prevCoords : snakeData[snakeData.get_size() - 1].coords;
        sf::Vector2f secondLastCoords = snakeData[snakeData.get_size() - 2].coords;
        sf::Vector2f newCoords = lastCoords;

        SnakeSegment snk;
        snk.coords = newCoords;
        snakeData.push_back(snk);
        ++score;
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

    bool update(sf::Time deltaTime) {
        static sf::Time elapsedTime;
        elapsedTime += deltaTime;

        if (elapsedTime.asMilliseconds() > 100) {
            if (checkCollision()) {
                std::cout << "Game over!" << std::endl;
                return false;
            }


            move();
            if (snakeData[0].coords == foodCoords) {
                add_snake();
                generateApple();
            }
            elapsedTime = sf::Time::Zero;
            return true;
        }
        else
            return true;
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




enum class states {
    MENU,
    GAME
};




class SnakeScreen : public sf::Drawable, public sf::Transformable {
private:
    bool beginWind, LeaveWind;

    states st;

    sf::Font LogoFont;
    sf::Text GameLogo;

    sf::Font ButtonFont;
    sf::RectangleShape startGameButtonBox;
    sf::Text startGame;

    sf::RectangleShape leaveGameButtonBox;
    sf::Text leaveGame;

    unsigned int max_score = 0;
    sf::Text ScoreText;
public:
    SnakeScreen() : LogoFont("C:/Windows/Fonts/arial.ttf"), GameLogo(LogoFont), ButtonFont("C:/Windows/Fonts/arial.ttf"), startGame(ButtonFont), leaveGame(ButtonFont), LeaveWind(false), beginWind(false), ScoreText(ButtonFont) {

		ScoreText.setCharacterSize(20);
		ScoreText.setPosition({ 20, 20 });
        ScoreText.setString("Max Score: " + std::to_string(max_score));

        GameLogo.setString("SNAKE GAME");
        GameLogo.setCharacterSize(50);
		GameLogo.setPosition({ 250, 100 });


        startGameButtonBox.setOrigin({ startGameButtonBox.getSize().x / 2 + 200, startGameButtonBox.getSize().y / 2 });
        startGameButtonBox.setFillColor(sf::Color::White);
        startGameButtonBox.setSize({ 400, 100 });
        startGameButtonBox.setPosition({ 400, 350 });
        startGame.setString("Start Game");
		startGame.setPosition({ 325, 375 });
        startGame.setFillColor(sf::Color::Red);


        leaveGameButtonBox.setOrigin({ leaveGameButtonBox.getSize().x / 2 + 200, leaveGameButtonBox.getSize().y / 2 });
        leaveGameButtonBox.setFillColor(sf::Color::White);
        leaveGameButtonBox.setSize({ 400, 100 });
        leaveGameButtonBox.setPosition({ 400, 475 });
        leaveGame.setString("Exit Game");
		leaveGame.setPosition({ 325, 500 });
        leaveGame.setFillColor(sf::Color::Red);
    }

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
        states.transform *= getTransform();

		target.draw(ScoreText, states);
        target.draw(GameLogo, states);

        target.draw(startGameButtonBox, states);
        target.draw(startGame, states);

        target.draw(leaveGameButtonBox, states);
        target.draw(leaveGame, states);
    }
    void MouseMoved(sf::Vector2i position) {
        if (startGameButtonBox.getGlobalBounds().contains({float(position.x), float(position.y)})) {
            beginWind = true;
            startGameButtonBox.setFillColor(sf::Color::Black);
        }

        if (!startGameButtonBox.getGlobalBounds().contains({ float(position.x), float(position.y) })) {
            beginWind = false;
            startGameButtonBox.setFillColor(sf::Color::White);
        }

        if (leaveGameButtonBox.getGlobalBounds().contains({ float(position.x), float(position.y) })) {
            LeaveWind = true;
            leaveGameButtonBox.setFillColor(sf::Color::Black);
        }

        if (!leaveGameButtonBox.getGlobalBounds().contains({ float(position.x), float(position.y) })) {
            LeaveWind = false;
            leaveGameButtonBox.setFillColor(sf::Color::White);
        }
    }

    bool getBeginWind() {
        return beginWind;
    }
	bool getLeaveWind() {
		return LeaveWind;
	}

    void MousePressed() {
        if (beginWind) {
            SnakeGame game;
            sf::RenderWindow window(sf::VideoMode({ 800, 600 }), "Snake game");
            sf::Clock clock;
			unsigned int score = 0;
            while (window.isOpen()) {
                while (const auto event = window.pollEvent()) {
                    if (event->is<sf::Event::Closed>()) {
                        window.close();
                    }
                    else if (const auto* button = event->getIf<sf::Event::KeyPressed>()) {
                        if (button->scancode == sf::Keyboard::Scancode::W || button->scancode == sf::Keyboard::Scancode::S || button->scancode == sf::Keyboard::Scancode::A || button->scancode == sf::Keyboard::Scancode::D)
                            game.move(*button);
                        else if (button->scancode == sf::Keyboard::Scancode::Escape) {
                            window.close();
                        }
                    }
                }
                if (!(game.update(clock.restart()))) {
					score = game.getScore();
                    if (score > max_score) {
                        max_score = score;
                        ScoreText.setString("Max Score: " + std::to_string(max_score));
                    }
                    window.close();
                }
                window.clear(sf::Color::Blue);
                window.draw(game);
                window.display();
            }
        }
    }
};



int main() {
	sf::RenderWindow window(sf::VideoMode({ 800, 600 }), "Snake game");
	SnakeScreen menu;
	sf::Clock clock;
    unsigned int max_score;
    max_score = 0;
	while (window.isOpen()) {
		while (const auto event = window.pollEvent()) {
			if (event->is<sf::Event::Closed>()) {
				window.close();
			}
			if (const auto* button = event->getIf<sf::Event::KeyPressed>()) {
				if (button->scancode == sf::Keyboard::Scancode::Escape) {
					window.close();
				}
			}
            else if (const auto* button = event->getIf<sf::Event::MouseMoved>()) {
                menu.MouseMoved(button->position);
            }
            else if (const auto* button = event->getIf<sf::Event::MouseButtonPressed>()) {
				if (button->button == sf::Mouse::Button::Left) {
                    if (menu.getLeaveWind()) {
                        window.close();
                        break;
                    }
                    else if (menu.getBeginWind())
                        menu.MousePressed();
				}
            }
		}
		window.clear(sf::Color::Blue);
		window.draw(menu);
		window.display();
	}

}