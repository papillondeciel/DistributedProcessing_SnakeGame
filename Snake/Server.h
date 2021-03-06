#pragma once
//#define WIN32_LEAN_AND_MEAN
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <queue>
#include "ownTypes.h"
#include "Queue.h"
#include "Data.h"

using namespace std;


class Server
{
	sf::TcpListener serverSocket; // gniazdo nasłuchujące
	vector < sf::TcpSocket *> clients; // tutaj przechowujemy klientów
	sf::SocketSelector sel; // selektor 

	Data globalData;

	Queue receivedPacketsQueue;
	Queue sentPacketsQueue;

	sf::Thread
		processingPacketsThread,
		receivingPacketsThread,
		sendingPacketsThread;

	sf::Mutex *mutex;

	volatile int counter;// = 0;
	int num_of_clients;// = 0;

	void processingPackets();
	void receivingPackets();
	void sendingPackets();

public:
	Server(unsigned int);

	void start();

	~Server();
};

