#include <cassert>
#include "Paddle.h"

void Paddle::add_message(const PaddleMessage& msg) {
	messages_.push_back(msg);
}

sf::Vector2f Paddle::predict_position(sf::Vector2f position, sf::Vector2f previous)
{
	predictedPos = position + (position - previous);

	return predictedPos;
}

void Paddle::predict_position(float time) {
	const int msize = messages_.size();
	assert(msize >= 3);
	const PaddleMessage& msg0 = messages_[msize - 1];
	const PaddleMessage& msg1 = messages_[msize - 2];
	const PaddleMessage& msg2 = messages_[msize - 3];
	float xVelocity;
	float yVelocity;


	// FIXME: Implement prediction here!
	// You have:
	// - the history of position messages received, in "messages_"
	//   (msg0 is the most recent, msg1 the 2nd most recent, msg2 the 3rd most recent)
	// - the current time, in "time"

	xVelocity = (msg0.pos.x - msg1.pos.x) / (msg0.time - msg1.time);
	yVelocity = (msg0.pos.y - msg1.pos.y) / (msg0.time - msg1.time);

	//X = aT + b
	//	x_ = xVelocity * (time - msg0.time) + msg0.x;
	//	y_ = yVelocity * (time - msg0.time) + msg0.y;

	//X = aT2 + bT + c
	predictedPos.x = (xVelocity * ((time - msg0.time) * (time - msg0.time))) + (msg1.pos.x *(time - msg0.time)) + msg2.pos.x;
	predictedPos.y = (yVelocity * ((time - msg0.time) * (time - msg0.time))) + (msg1.pos.y *(time - msg0.time)) + msg2.pos.y;

	body.setPosition(predictedPos);
}

void Paddle::move(float xChange, float yChange)
{

	position.x += xChange;
	position.y += yChange;

	//check boundaries
	if (position.x - body.getRadius() < 0)
	{
		position.x = 0 + body.getRadius();
	}
	if (position.x + body.getRadius() > 800)
	{
		position.x = 800 - body.getRadius();
	}
	if (position.y - body.getRadius() < 0)
	{
		position.y = 0 + body.getRadius();
	}
	if (position.y + body.getRadius() > 600)
	{
		position.y = 600 - body.getRadius();
	}
	

	body.setPosition(position);
}

sf::Vector2f Paddle::getPosition()
{
	return position;
}

sf::CircleShape Paddle::getBody()
{
	return body;
}

void Paddle::Init(sf::Vector2f pos)
{
	//initialise the variables
	position = pos;
	body.setPosition(position);
	predictedPos = pos;
	previousPos = pos;
	body.setRadius(15.0f);
	body.setOutlineThickness(2);
	body.setOutlineColor(sf::Color::Black);
}