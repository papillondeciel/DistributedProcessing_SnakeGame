#include "Client.h"
#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <thread>
#include <Windows.h>
#include <process.h>
#include <conio.h>
#include "ownTypes.h"
#include "Direction.h"
#include "FromClientToServerData.h"
#include "FromServerToClientData.h"
#include "InitializingData.h"



using namespace std;


Client::Client(unsigned int port) :
	init(false),
	myID(0),
	processingReceivedPacketsThread(&Client::processingReceivedPackets, this),
	receivingPacketsThread(&Client::receivingPackets, this),
	sendingPacketsThread(&Client::sendingPackets, this),
	processingPlayerInteractionsThread(&Client::processingPlayerInteractions, this),
	drawingThread(&Client::drawing, this)
{
	this->mutex = new sf::Mutex;
	sf::IpAddress ip = "localhost";
	//cerr << " moj port " << socket.getLocalPort() << endl;
	while (true)
	{
		if (socket.connect(ip, port) != sf::Socket::Done) // ³¹czymy siê z adresem 'ip' na porcie 'port'											  // jeœli funkcja connect zwróci sf::Socket::Done oznacza to, ¿e wszystko posz³o dobrze
		{
			cerr << "C: Nie mozna polaczyæ sie z " << ip.toString() << endl;
			//exit(1);
		}
		else
		{
			cerr << "C: Polaczylem sie!!!" << endl;
			cerr << "C: moj port na korym sie polaczylem to: " << socket.getLocalPort() << endl;
			break;
		}
	}

}

void Client::start()
{
	this->receivingPacketsThread.launch();
	this->processingReceivedPacketsThread.launch();
	this->sendingPacketsThread.launch();
	this->processingPlayerInteractionsThread.launch();
	this->drawingThread.launch();
}


void Client::processingReceivedPackets()
{
	bool empty = false;
	InitializingData *data;// = new Data();
	while (true)
	{
		//std::this_thread::sleep_for(std::chrono::seconds(DELAY_IN_SHOW_LOGS));
		//sf::Lock lock(this->mutex); 
		this->mutex->lock();
		empty = this->receivedPacketsQueue.empty();
		this->mutex->unlock();

		if (!empty)
		{
			this->mutex->lock();
			data = (InitializingData*)this->receivedPacketsQueue.popFirst();// front();
														 //this->receivedPacketsQueue.pop();

			this->mutex->unlock();

			/*cout << "Odebrano dane: " << endl;
			data.writeMatrix();
			cout << " & " << endl;
			data.writeDirectionsTable();*/
			///cout << "C: procesuje dane ---> i tu kminie rzeczy" << endl;
			if (this->myID == 0)
			{
				cout << "C: odebralem info inicjalizyjace" << endl;
				///odbieram paczke inizjalizujaca
				this->myID = data->getClientIDinitializingData();
				cout << "C: moje ID: " << this->myID /*<< " " << data->getClientIDinitializingData()*/ << endl;
				this->oldDirection.setDestination(data->getInitDirection_forClient().getDestination());
				this->oldDirection.position = data->getInitDirection_forClient().position;
				this->newDirection = this->oldDirection;

			}
			else
			{
				cout << "C: odebralem info regularne :D " << endl;
				FromServerToClientData *regularData = (FromServerToClientData*)data;
				globalData = *regularData;
				//od teraz mozemy zczytywac plansze i wszystkie dane o kierunkach i polozenu glow z globalData
				//TODO - reakcja na smierc
			}
			//" od adres: " << temp2.remoteAdressOfClient << " port:" << temp2.remotePortOfClient << endl;
		}
		//this->mutex->unlock();
	}
}

void Client::receivingPackets()
{
	BaseData *receiveData;
	while (true)
	{
		sf::Packet pack;
		if (this->socket.receive(pack) == sf::Socket::Done)
		{
			if (!this->init)
			{
				InitializingData* initData = new InitializingData();// = (InitializingData*)receiveData;
				initData->unpack(pack);
				receiveData = (BaseData*)initData;
				//cout << "C: odebrane ID: " << receiveData->getClientIDinitializingData() << endl<< endl;
				this->init = true;
			}
			else
			{
				FromServerToClientData* fromServerData = new FromServerToClientData();// = (FromServerToClientData*)receiveData;
				fromServerData->unpack(pack);
				receiveData = (BaseData*)fromServerData;
			}

			//sf::Lock lock(this->mutex); 
			this->mutex->lock();
			this->receivedPacketsQueue.push(receiveData);
			this->mutex->unlock();

		}
	}
}

void Client::sendingPackets()
{

	FromClientToServerData * sendData = new FromClientToServerData();
	bool empty = false;


	while (true)
	{
		//std::this_thread::sleep_for(std::chrono::seconds(DELAY_IN_SHOW_LOGS));
		//sf::Lock lock(this->mutex);
		this->mutex->lock();
		empty = this->sentPacketsQueue.empty();
		this->mutex->unlock();

		if (!empty)
		{
			this->mutex->lock();
			sendData = (FromClientToServerData*)(this->sentPacketsQueue.popFirst());// front();
														 //this->sentPacketsQueue.pop();
			this->mutex->unlock();
			sf::Packet pack;
			sendData->pack(pack);
			//	pak << d.text << d.num;
			if (socket.send(pack) != sf::Socket::Done)
			{
				// nie mo¿na wys³aæ danych (prawdopodobnie klient/serwer siê roz³¹czy³)
				cerr << "C: Nie mo¿na wys³aæ danych!\n";
				//break;
			}
			else
			{
				cout << "C: Wyslano dane... " << endl;// : " << d.text << " & " << d.num << " do serwera, moj port " << socket.getLocalPort() << " remote port " << socket.getRemotePort() << endl;
													  //break;
			}

			//}
		}
		//this->mutex->unlock();
	}
}

void Client::processingPlayerInteractions()
{
	HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
	DWORD NumInputs = 0;
	DWORD InputsRead = 0;
	//bool running = true;

	INPUT_RECORD irInput;

	Direction::destination_t direct;
	bool setNewDirect = false;
	FromClientToServerData *tempData = new FromClientToServerData();
	Direction tempDir;

	GetNumberOfConsoleInputEvents(hInput, &NumInputs);

	while (true)
	{
		ReadConsoleInput(hInput, &irInput, 1, &InputsRead);
		if (irInput.Event.KeyEvent.bKeyDown)
		{
			this->oldDirection = this->newDirection;
			switch (irInput.Event.KeyEvent.wVirtualKeyCode)
			{
			case VK_ESCAPE:
				cout << "Escape" << endl;
				break;

			case VK_LEFT:
				cout << "Left" << endl;
				direct = Direction::destination_t::LEFT;
				setNewDirect = true;
				break;

			case VK_UP:
				cout << "Up" << endl;
				direct = Direction::destination_t::UP;
				setNewDirect = true;
				break;

			case VK_RIGHT:
				cout << "Right" << endl;
				direct = Direction::destination_t::RIGHT;
				setNewDirect = true;
				break;

			case VK_DOWN:
				cout << "Down" << endl;
				direct = Direction::destination_t::DOWN;
				setNewDirect = true;
				break;
			}

			if (setNewDirect)
			{
				this->newDirection.setDestination(direct);
				tempData->setFromClientToServerData(this->myID, { 0,0 }, this->oldDirection, this->newDirection);
				this->mutex->lock();
				this->sentPacketsQueue.push((BaseData*)tempData);
				this->mutex->unlock();
				setNewDirect = false;
			}
		}
	}
}



//nie testowalam czy skrecanie dziala bo nie mialam jak
//mam nadzieje ze to co bylo do wzieca z kodu Pauli to wzielam, ale mozliwe ze jeszcze cos z tamtad bedzie potrzebne

void Client::drawing()
{
	const float body_size = 25;
	sf::Color snakeColor = sf::Color::Green;
	sf::Color snakeOutlineColor = sf::Color::White;
	sf::Color backgroundColor = sf::Color::Green;

	const float speed = 10;
	sf::Clock clock;
	sf::Time t1 = clock.getElapsedTime();

	sf::RenderWindow window(sf::VideoMode(BOARD_SIZE*body_size, BOARD_SIZE*body_size), "Gold Snake");
	window.setFramerateLimit(30);
	window.clear(backgroundColor);

	sf::RectangleShape segment(sf::Vector2f(body_size, body_size));
	segment.setFillColor(snakeColor);
	segment.setOutlineThickness(-1);
	segment.setOutlineColor(snakeOutlineColor);
	

	//TODO tutaj poczekac az dostaniemy info inicjalizujace !!

	while (window.isOpen())
	{

		while (t1.asSeconds() >= 1 / speed)
		{
			t1 = clock.getElapsedTime(); //waiting loop
		}
		clock.restart();


		//petla do obliczenia nowej pozycji wezy:
		for (int i = 0; i < MAX_NUM_OF_CLIENTS; i++)
		{
			this->mutex->lock();
			Direction::point_t headposition = globalData.getPlayerHeadPosition(i + 1);
			Direction::point_t newHeadposition = headposition;
			Direction dir = globalData.getPlayerDirection_forServer(i + 1);
			this->mutex->unlock();
			switch (dir.getDestination())
			{
			case Direction::destination_t::UP:
				newHeadposition.y--; break;
			case Direction::destination_t::DOWN:
				newHeadposition.y++; break;
			case Direction::destination_t::LEFT:
				newHeadposition.x--; break;
			case Direction::destination_t::RIGHT: default:
				newHeadposition.x++; break;
			}
			//kolizja z krawedzią:
			if (newHeadposition.x < 0 || newHeadposition.y < 0 || newHeadposition.x >= BOARD_SIZE || newHeadposition.y >= BOARD_SIZE);
			else
			{
				this->mutex->lock(); //not sure if globalData needs mutex protection
				globalData.setPlayerHeadPosition(i + 1, newHeadposition);
				globalData.setPlayerHeadOnBoard(i + 1, newHeadposition);
				this->mutex->unlock();
			}
		}


		//drawing map
		window.clear(backgroundColor);
		for (int i = 0; i < BOARD_SIZE; i++)
		{
			for (int j = 0; j < BOARD_SIZE; j++)
			{
				Direction::point_t point = { i, j };
				unsigned int cellValue = globalData.getCellValueFromBoardMatrix(point);
				if (cellValue != 0) //TODO dodac rozroznianie graczy
				{
					segment.setPosition(sf::Vector2f(i*body_size, j*body_size));
					window.draw(segment);
				}
								
			}
		}

		window.display();

	}
}


Client::~Client()
{
}