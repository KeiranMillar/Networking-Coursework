#include "Client.h"
#include "messages.h"
#include "Paddle.h"

using std::cin;
using std::cout;

using namespace std;

sf::Vector2f Lerp(sf::Vector2f pos1, sf::Vector2f pos2, float t)
{
	sf::Vector2f move;
	float time = t / 10.0f;

	move = pos1 + t * (pos2 - pos1);

	return move;
}

int main()
{
	sf::IpAddress ip = sf::IpAddress::getLocalAddress();
	//	std::string publicIP = ip.toString();

	sf::Font font;
	if (!font.loadFromFile("../resources/sansation.ttf"))
		return EXIT_FAILURE;

	// Initialize the pause message
	sf::Text pauseMessage;
	pauseMessage.setFont(font);
	pauseMessage.setCharacterSize(40);
	pauseMessage.setPosition(170.f, 150.f);
	pauseMessage.setFillColor(sf::Color::White);
	pauseMessage.setString("Welcome to my Network coursework!\nPlease wait for connection");

	sf::TcpSocket socket;

	bool done = false;
	bool update = false;
	bool connected = false;
	int playerNum = -1;
	float currentTime = 0;
	float lerpDuration = 0;
	float deltaTime;
	float speed = 100000.0f;

	Paddle players[2];
	sf::Clock clock;

	players[0].Init(sf::Vector2f(0, 200));
	players[1].Init(sf::Vector2f(200, 200));

	players[0].body.setFillColor(sf::Color::Blue);
	players[1].body.setFillColor(sf::Color::Red);

	std::string name;
	std::string messageType;

	std::cout << "Enter name:" << std::endl;
	cin >> name;

	//ip = sf::IpAddress(publicIP);
	socket.connect(ip, 2000);

	sf::RenderWindow window(sf::VideoMode(800, 600, 32), "Networking");
	window.setTitle(name);

	sf::Packet packet;
	messageType = "NAME";
	packet << messageType;
	socket.setBlocking(false);

	//Send the server the users name
	if (socket.send(packet) == sf::Socket::Done)
	{
		packet.clear();
		packet << name;
		socket.send(packet);
	}

	//recieve the user id from the server
	packet.clear();
	window.draw(pauseMessage);
	socket.receive(packet);
	packet >> playerNum;

	while (connected == false)
	{
		packet.clear();
		socket.receive(packet);
		packet >> messageType;
		if (messageType == "Go")
		{
			//start the game when there is enough users connected
			connected = true;
			cout << "Let's Go!" << endl;
		}
	}

	//set the players previous position so that the preicition works on start up
	sf::Vector2f prevPosition = players[playerNum].getPosition();

	//game loop
	while (!done)
	{
		sf::Event Event;
		deltaTime = clock.restart().asSeconds();
		while (window.pollEvent(Event))
		{
			//get user keystrokes
			if (Event.type == sf::Event::Closed || Event.key.code == sf::Keyboard::Escape)
			{
				window.close();
			}
			else if (Event.type == sf::Event::GainedFocus)
			{
				update = true;
			}
			else if (Event.type == sf::Event::LostFocus)
			{
				update = false;
			}
			prevPosition = players[playerNum].getPosition();
			if (update)
			{
				//move the player in the desired direction
				switch (Event.type)
				{
				case sf::Event::Closed:
					window.close();
					break;
				case sf::Event::KeyPressed:
					if (Event.key.code == sf::Keyboard::Escape)
						window.close();
					if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
					{
						players[playerNum].move(0.0f, -speed *deltaTime);
					}
					if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
					{
						players[playerNum].move(0.0f, speed *deltaTime);
					}
					if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
					{
						players[playerNum].move(-speed *deltaTime, 0.0f);
					}
					if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
					{
						players[playerNum].move(speed *deltaTime, 0.0f);
					}
					break;
				}
			}
		}
		if (prevPosition != players[playerNum].getPosition())
		{
			//send the server the users position if the user moves
			sf::Packet packet;
			messageType = "playerInfo";
			packet << messageType;

			if (socket.send(packet) == sf::Socket::Done)
			{
				packet.clear();
				packet << playerNum << players[playerNum].getPosition().x << players[playerNum].getPosition().y;
				socket.send(packet);
				packet.clear();
			}
			currentTime = 0.0f;

		}

		sf::Packet packet;		
		if (socket.receive(packet) == sf::Socket::Done)
		{
			//recieve the other players position and predict their next position
			sf::Vector2f recievedPos;
			sf::Vector2f recievedPos2;
			packet >> recievedPos.x >> recievedPos.y;
			packet.clear();
			if (playerNum == 0)
			{
				players[1].previousPos = players[1].position;
				players[1].position = recievedPos;
				players[1].predictedPos = players[1].predict_position(players[1].position, players[1].previousPos);
				players[1].position = (Lerp(players[1].position, players[1].predictedPos, deltaTime));
				players[1].body.setPosition(players[1].predictedPos);
			}
			if (playerNum == 1)
			{
				players[0].previousPos = players[0].position;
				players[0].position = recievedPos;
				players[0].predictedPos = players[0].predict_position(players[0].position, players[0].previousPos);
				players[0].position = (Lerp(players[0].position, players[0].predictedPos, deltaTime));
				players[0].body.setPosition(players[0].position);
			}
		}

		//if you don't recieve a position then just use their last send position to further predict
		if (playerNum == 0)
		{
			players[1].previousPos = players[1].position;
			players[1].predictedPos = players[1].predict_position(players[1].position, players[1].previousPos);
			players[1].position = (Lerp(players[1].position, players[1].predictedPos, deltaTime));
			players[1].body.setPosition(players[1].predictedPos);
		}
		if (playerNum == 1)
		{
			players[0].previousPos = players[0].position;
			players[0].predictedPos = players[0].predict_position(players[0].position, players[0].previousPos);
			players[0].position = (Lerp(players[0].position, players[0].predictedPos, deltaTime));
			players[0].body.setPosition(players[0].position);
		}

		//increment time
		currentTime += deltaTime;
		lerpDuration += deltaTime;

		if (lerpDuration > 0.5f)
		{
			lerpDuration = 0.0f;
		}

		//draw the display
		window.draw(players[0].getBody());
		window.draw(players[1].getBody());
		window.display();
		window.clear(sf::Color::White);
	}


	return 0;
}