#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <fstream>
#include <iostream>
#include <vector>

class timer {
public:

	sf::Clock clock0;
	float m_fTime;
	float m_accTime = 0;

	float frame_time() {
		sf::Time elapsed_time;
		sf::Time delta_time;

		float dt_seconds;

		delta_time = clock0.restart();
		elapsed_time += delta_time;

		dt_seconds = delta_time.asSeconds();

		return dt_seconds;
	}

	float accumulate_time() {
		m_fTime = frame_time();
		m_accTime = m_accTime + m_fTime;
		return m_accTime;
	}
};

struct collisionDetector {

	bool position_above = false;
	bool position_right = false;
	bool position_left = false;
	bool position_below = false;

	bool collision = false;

};

class Environment {
public:
	sf::RectangleShape shape;

	collisionDetector cd;

	sf::Vector2f size;
	sf::Vector2f position;

	Environment(sf::Vector2f s, sf::Vector2f p) {};

};

class Platform : public Environment {
	public:

	Platform(sf::Vector2f s, sf::Vector2f p) :Environment(s, p) {
		shape.setPosition(sf::Vector2f(rand() % 150 + 50, rand() % 300 + 200));
		shape.setSize(sf::Vector2f(rand() % 150 + 50, rand() % 50 + 1));
		shape.setOrigin(shape.getSize().x / 2, shape.getSize().y / 2);
	}
};


class Player {
public:
	sf::RectangleShape shape;

	float vertical_velocity = 0;
	float acceleration = 0.0001;
	int vertical_direction = 1;
	int horizontal_direction = 0;

	int h_multiplier = 450;
	int v_multiplier = 950000;

	timer t;

	Player() {
		shape.setSize(sf::Vector2f(20, 30));
		shape.setOrigin(sf::Vector2f(10, 15));
		shape.setFillColor(sf::Color(255, 100, 0));
		shape.setPosition(sf::Vector2f(100, 250));
	}

	void movement(std::vector <Platform> platforms) {

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
			horizontal_direction = -1;
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
			horizontal_direction = 1;
		}
		else{ horizontal_direction = 0; }
		for (int i = 0; i < platforms.size(); i++) {
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) && platforms[i].cd.position_above && platforms[i].cd.collision) {
				vertical_direction = -1;
				vertical_velocity = 0.40;
			}
		}

		for (int i = 0; i < platforms.size(); i++) {
			if (vertical_direction == -1) { vertical_velocity -= acceleration; }
			else if (!platforms[i].cd.collision && vertical_direction == 0) { vertical_velocity = 0; vertical_direction = 1; }
			else { vertical_velocity += acceleration; }
		}
		if (vertical_velocity < 0 && vertical_direction == -1) { vertical_velocity = vertical_velocity * (-1); vertical_direction = 1; }

		shape.move(sf::Vector2f(horizontal_direction * (t.frame_time() * h_multiplier), vertical_velocity * vertical_direction * (t.frame_time() * v_multiplier)));
	}

};

void detect_collision(Player& player, std::vector <Platform>& platforms) {
	int i;
	for (i = 0; i < platforms.size(); i++) {

		platforms[i].cd.position_above = platforms[i].shape.getPosition().y - player.shape.getPosition().y >= player.shape.getOrigin().y + platforms[i].shape.getOrigin().y - 5;
		platforms[i].cd.position_left = platforms[i].shape.getPosition().x - player.shape.getPosition().x >= player.shape.getOrigin().x + platforms[i].shape.getOrigin().x - 5;
		platforms[i].cd.position_right = player.shape.getPosition().x - platforms[i].shape.getPosition().x >= player.shape.getOrigin().x + platforms[i].shape.getOrigin().x - 5;
		platforms[i].cd.position_below = player.shape.getPosition().y - platforms[i].shape.getPosition().y >= player.shape.getOrigin().y + platforms[i].shape.getOrigin().y - 5;

		platforms[i].cd.collision = abs(player.shape.getPosition().x - platforms[i].shape.getPosition().x) < (player.shape.getOrigin().x + platforms[i].shape.getOrigin().x) && abs(player.shape.getPosition().y - platforms[i].shape.getPosition().y) < (player.shape.getOrigin().y + platforms[i].shape.getOrigin().y);

	}
	//return cd;
}

int main() {

	//handle events here

	Player player;

	std::vector <Platform> platforms;

	srand(time(NULL));
	platforms.push_back(Platform({ (float)(rand() % 150 + 50), (float)(rand() % 300 + 200) }, { (float)(rand() % 150 + 50), (float)(rand() % 50 + 1) }));
	platforms.push_back(Platform({ (float)(rand() % 150 + platforms[0].shape.getPosition().x + 150), (float)(rand() % 300 + 200) }, { (float)(rand() % 150 + 50), (float)(rand() % 50 + 1) }));
	platforms.push_back(Platform({ (float)(rand() % 300 + platforms[1].shape.getPosition().x + 10), (float)(rand() % 300 + 200) }, { (float)(rand() % 50 + platforms[1].shape.getSize().x), (float)(rand() % 50 + platforms[1].shape.getSize().y) }));

	platforms[0].shape.setFillColor(sf::Color(100, 100, 255));
	platforms[1].shape.setFillColor(sf::Color(100, 100, 255));
	platforms[2].shape.setFillColor(sf::Color(100, 255, 100));

	player.shape.setPosition(sf::Vector2f(platforms[0].shape.getPosition().x, platforms[0].shape.getPosition().y - platforms[0].shape.getOrigin().y - player.shape.getOrigin().y));

	sf::RenderWindow w(sf::VideoMode(700, 500), "Platformer Framework");
	sf::Event e;

	//handle events here
	while (w.isOpen()) {
		while (w.pollEvent(e)) {
			switch (e.type) {
			case sf::Event::Closed:
				w.close();
				break;
			}
		}

		//update game logic here

		player.movement(platforms);

		detect_collision(player, platforms);

		for (int i = 0; i < platforms.size(); i++) {
			if (platforms[i].cd.position_above && platforms[i].cd.collision) { player.shape.setPosition(sf::Vector2f(player.shape.getPosition().x, (platforms[i].shape.getPosition().y - (player.shape.getOrigin().y + platforms[i].shape.getOrigin().y)))); player.vertical_velocity = 0; }
			if (platforms[i].cd.position_left && platforms[i].cd.collision) { player.shape.setPosition(sf::Vector2f(platforms[i].shape.getPosition().x - (player.shape.getOrigin().x + platforms[i].shape.getOrigin().x), player.shape.getPosition().y)); }
			if (platforms[i].cd.position_right && platforms[i].cd.collision) { player.shape.setPosition(sf::Vector2f(platforms[i].shape.getPosition().x + (player.shape.getOrigin().x + platforms[i].shape.getOrigin().x), player.shape.getPosition().y)); }
			if (platforms[i].cd.position_below && platforms[i].cd.collision) { player.shape.setPosition(sf::Vector2f(player.shape.getPosition().x, (platforms[i].shape.getPosition().y + (player.shape.getOrigin().y + platforms[i].shape.getOrigin().y)))); player.vertical_velocity = 0; player.vertical_direction = 1; }
		}

		if ((platforms[2].cd.position_above && platforms[2].cd.collision) || player.shape.getPosition().y >= 615) {
			
			platforms[0].shape.setPosition(sf::Vector2f(rand() % 150 + 50, rand() % 300 + 200));
			platforms[1].shape.setPosition(sf::Vector2f(rand() % 150 + platforms[0].shape.getPosition().x + 150, rand() % 300 + 200));
			platforms[2].shape.setPosition(sf::Vector2f(rand() % 300 + platforms[1].shape.getPosition().x + 10, rand() % 300 + 200));

			platforms[0].shape.setSize(sf::Vector2f(rand() % 150 + 50, rand() % 50 + 1));
			platforms[1].shape.setSize(sf::Vector2f(rand() % 150 + 50, rand() % 50 + 1));
			platforms[2].shape.setSize(sf::Vector2f(rand() % 50 + platforms[1].shape.getSize().x, rand() % 50 + platforms[1].shape.getSize().y));

			for (int i = 0; i < platforms.size(); i++) {
				platforms[i].shape.setOrigin(platforms[i].shape.getSize().x / 2, platforms[i].shape.getSize().y / 2);
			}

			platforms[0].shape.setFillColor(sf::Color(100, 100, 255));
			platforms[1].shape.setFillColor(sf::Color(100, 100, 255));
			platforms[2].shape.setFillColor(sf::Color(100, 255, 100));
			player.vertical_velocity = 0;
			player.shape.setPosition(sf::Vector2f(platforms[0].shape.getPosition().x, platforms[0].shape.getPosition().y - platforms[0].shape.getOrigin().y - player.shape.getOrigin().y));

		}
		
		w.clear(sf::Color::White);

		//draw objects here
		w.draw(player.shape);
		for (int i = 0; i < platforms.size(); i++) { w.draw(platforms[i].shape); }
		//draw objects here

		w.display();
	}
}





