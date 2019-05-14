#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include<SFML/Network.hpp>
#include<SFML/Graphics/Font.hpp>

#include <cmath>
#include <ctime>
#include <cstdlib>
#include<string>
#include<iostream>
#include<map>
#include<conio.h>

using namespace std;

//gamestate
enum STATE {
	CONNECT,
	PLAY,
	QUIT
};

int main()
{
std:cout << "Server Running " << std::endl;

	sf::TcpListener listener;
	sf::SocketSelector selector;
	bool done = false;
	int ID;
	std::string name;
	STATE gameState = CONNECT;


	//Position of the player
	sf::Vector2f Player0Position(0, 200);
	sf::Vector2f Player1Position(200, 200);
	std::vector<sf::TcpSocket*> clients;
	std::string messageType;

	listener.listen(2000);
	selector.add(listener);

	while (true)
	{
		switch (gameState) 
		{
		case CONNECT:
		{
			if (selector.wait())
			{
				//If someone is trying to connnect
				if (selector.isReady(listener))
				{
					//Create a pointer
					sf::TcpSocket * socket = new sf::TcpSocket;
					listener.accept(*socket);
					sf::Packet packet;

					//Recieve a message to say what is about to be sent
					if (socket->receive(packet) == sf::Socket::Done)
					{
						packet >> messageType;
						packet.clear();
						if (messageType == "NAME")
						{
							sf::Packet packetinfo;
							if (socket->receive(packetinfo) == sf::Socket::Done)
							{
								packetinfo >> name;
								cout << name << " has Connected!" << endl;
							}
						}
					}

					//Add to client vector
					clients.push_back(socket);
					//Add to selector(using pointer)
					selector.add(*socket);

					for (int i = 0; i < clients.size(); i++)
					{
						if (clients[i] == socket)
						{
							sf::Packet sendID;
							sendID << i;
							clients[i]->send(sendID);
						}
					}
					if (clients.size() == 2)
					{
						for (int j = 0; j < clients.size(); j++)
						{
							packet.clear();
							messageType = "Go";
							packet << messageType;
							clients[j]->send(packet);
						}
						gameState = PLAY;
					}
				}
			}
			break;
		}
		case PLAY:
		{
			//wait for a message
			if (selector.wait())
			{
				for (int i = 0; i < clients.size(); i++)
				{
					if (selector.isReady(*clients[i]))
					{
						sf::Packet packet, sendPacket[2];
						if (clients[i]->receive(packet) == sf::Socket::Done)
						{
							//if ther message recieved is playerInfo then get ready to take in the variables
							packet >> messageType;
							packet.clear();
							if (messageType == "playerInfo")
							{
								if (clients[i]->receive(packet) == sf::Socket::Done)
								{
									sf::Vector2f tempPos;
									packet >> ID >> tempPos.x >> tempPos.y;
									packet.clear();
									if (ID == 0)
									{
										Player0Position = tempPos;
									}
									if (ID == 1)
									{
										Player1Position = tempPos;
									}
									packet.clear();
								}
							}

							//set the players positions
							sendPacket[0] << Player0Position.x << Player0Position.y;
							sendPacket[1] << Player1Position.x << Player1Position.y;

							for (int j = 0; j < clients.size(); j++)
							{
								if (i != j)
								{
									//send all the users the positions of the other player
									clients[j]->send(sendPacket[i]);
								}
							}
						}
					}
				}
			}
			break;
		}
		case QUIT:
		{
			//std::cout << id << " has disconnected" << std::endl;
			for (std::vector<sf::TcpSocket*>::iterator it = clients.begin(); it != clients.end(); it++)
			{
				delete*it;
			}
			break;
		}
		}
	}
	return 0;
}