#ifndef PADDLEMESSAGE_H
#define PADDLEMESSAGE_H

#include "Client.h"

/** A position update message. */
struct PaddleMessage {
	/** The object number of this tank within the game world. */
	int id;

	/** The coordinates of this tank within the game world. */
	sf::Vector2f pos;

	/** The time at which this message was sent.
	(Not the time at which it was received!) */
	float time;
};

#endif