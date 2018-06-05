#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <queue>
#include "ownTypes.h"
#include "Queue.h"

using namespace std;


class Client
{
	sf::TcpSocket socket; // tworzymy gniazdo klienta
	sf::IpAddress ip;// = "localhost";

	bool init;
	unsigned int myID;

	//Data globalData;

	Direction newDirection;
	Direction oldDirection;

	Queue receivedPacketsQueue;
	Queue sentPacketsQueue;

	sf::Thread
		processingReceivedPacketsThread,
		receivingPacketsThread,
		sendingPacketsThread,
		processingPlayerInteractionsThread;

	sf::Mutex *mutex;

	//volatile int counter;// = 0;
	//int num_of_clients;// = 0;

	void processingReceivedPackets();
	void receivingPackets();
	void sendingPackets();
	void processingPlayerInteractions();

public:
	Client(unsigned int);

	void start();

	~Client();
};

