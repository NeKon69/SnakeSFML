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

	struct snake{
		sf::Vector2f coords;
		bool head = false;
		snake* next = nullptr;
	};
	unsigned int size = snake_data.get_size();
	sf::Vector2f				  prev_coords;
	raw::vector<snake>			   snake_data;


	sf::Vector2f food_coords;

	sf::Keyboard::Scancode prev_move;
	
	std::random_device			   rd;
	std::mt19937				  gen;
	std::uniform_int_distribution<> x;
	std::uniform_int_distribution<> y;
public:
	SnakeGame() : snake_data(1), gen(rd()), x(0, 799), y(0, 599) {
		snake_data.reserve(96000);
		snake_data[0].coords = sf::Vector2f(100, 100);
		snake_data[0].head = true;
	}

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
		states.transform *= getTransform();
		for (auto& i : snake_data) {
			sf::RectangleShape rect({ 10, 10 });
			if (i.head)
				rect.setFillColor(sf::Color::Red);
			else
				rect.setFillColor(sf::Color::Green);
			rect.setPosition(i.coords);
			target.draw(rect, states);
		}
		sf::CircleShape food_coords_(5);
		food_coords_.setPosition(food_coords);
		target.draw(food_coords_, states);
	}

	void move(const sf::Keyboard::Scancode& button) {
		prev_move = button;
	}
	void move(const sf::Event::KeyPressed& button) {
		prev_move = button.scancode;
	}

	void move() {
		if (size == 1) prev_coords = snake_data[0].coords;
		if (prev_move == sf::Keyboard::Scancode::A) {
			if (prev_move != sf::Keyboard::Scancode::D)
				snake_data[0].coords.x -= 10;
		}

		if (prev_move == sf::Keyboard::Scancode::D) {
			if (prev_move != sf::Keyboard::Scancode::A)
				snake_data[0].coords.x += 10;
		}
		if (prev_move == sf::Keyboard::Scancode::W) {
			if (prev_move != sf::Keyboard::Scancode::S)
				snake_data[0].coords.y -= 10;
		}
		if (prev_move == sf::Keyboard::Scancode::S) {
			if (prev_move != sf::Keyboard::Scancode::W)
				snake_data[0].coords.y += 10;
		}
		for (int i = snake_data.get_size() - 1; i > 0; --i) {
			snake_data[i].coords = snake_data[i - 1].coords;
		}
	}

	void add_snake() {
		sf::Vector2f lastCoords = snake_data[snake_data.get_size() - 1].coords;
		sf::Vector2f secondLastCoords = size == 1 ? prev_coords : snake_data[snake_data.get_size() - 2].coords;

		sf::Vector2f newCoords = lastCoords;
		if (lastCoords.x == secondLastCoords.x) {
			if (lastCoords.y > secondLastCoords.y)
				newCoords.y += 10;
			else
				newCoords.y -= 10;
		}
		else if (lastCoords.y == secondLastCoords.y) {
			if (lastCoords.x > secondLastCoords.x)
				newCoords.x += 10;
			else
				newCoords.x -= 10;
		}
		snake snk;
		snk.coords = newCoords;
		snk.next = &(snake_data[snake_data.get_size() - 1]);
		snake_data.push_back(snk);
		++size;
	}

	void generate_apple() {
		float x_ = x(gen);
		float y_ = y(gen);
		food_coords = { x_, y_ };
	}

	void update(sf::Time te) {
		if (te.asMilliseconds() > 100) {
			move();
			if (snake_data[0].coords == food_coords) {
				add_snake();
				generate_apple();
			}
		}

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