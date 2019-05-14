#ifndef PADDLE_H
#define PADDLE_H

#include <vector>

#include "messages.h"

class Paddle {
public:

	void add_message(const PaddleMessage& msg);
	/** Compute the predicted position at the given time,
	based on messages_, updating x_ and y_. */
	void predict_position(float time);

	sf::Vector2f predict_position(sf::Vector2f position, sf::Vector2f previous);

	void move(float xChange, float yChange);

	sf::Vector2f getPosition();

	sf::CircleShape getBody();

	sf::CircleShape body;

	void Init(sf::Vector2f pos);

	// Accessors.
	float x() const { return predictedPos.x; }
	float y() const { return predictedPos.y; }

	sf::Vector2f position;

	/** The current predicted position of this player. */
	sf::Vector2f predictedPos;

	/** The previous position of this player */
	sf::Vector2f previousPos;

private:
	

	/** The messages that have been received about this player. */
	std::vector<PaddleMessage> messages_;
};

#endif