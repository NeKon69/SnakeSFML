#include <SFML/Graphics.hpp>
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
		snake* previous = nullptr;
	};

	raw::vector<snake> snake_data;
	sf::Vector2f food_coords;
	unsigned int size = snake_data.get_size();

public:
	SnakeGame() : snake_data(1) {
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
	}

	void move(sf::Keyboard::Scancode& button) {
		for (int i = 0; i < snake_data.get_size(); ++i) {
			if (button == sf::Keyboard::Scancode::A) {

			}
		}
	}
};