#include "Server.h"
#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <thread>
#include <Windows.h>
#include <process.h>
#include "ownTypes.h"
#include "Direction.h"
#include "FromClientToServerData.h"
#include "FromServerToClientData.h"
#include "InitializingData.h"

using namespace std;

Server::Server(unsigned int port) :
	num_of_clients(0),
	counter(0),
	processingPacketsThread(&Server::processingPackets, this),
	receivingPacketsThread(&Server::receivingPackets, this),
	sendingPacketsThread(&Server::sendingPackets, this),
	timeCountingCollisionCheckingThread(&Server::timeCountingCollisionChecking, this)
{
	this->mutex = new sf::Mutex;

	if (this->serverSocket.listen(port) != sf::Socket::Done) // rozpoczynamy nasłuchiwanie na porcie 'port'
	{
		cerr << "S: Nie moge rozpoczac nasluchiwania na porcie " << port << endl;
		//exit(1);
	}

	cout << "Moj lokalny port to:" << this->serverSocket.getLocalPort() << endl;
	this->sel.add(serverSocket); // dodajemy gniazdo nasłuchujące

}

void Server::start()
{
	srand(time(NULL));
	while (true && num_of_clients < MAX_NUM_OF_CLIENTS)
	{	// pętla główna serwera

		if (this->sel.wait(sf::seconds(2))) // jeśli metoda wait() zwróci true, to znaczy, że któreś z dodanych gniazd jest gotowe do odbioru
		{							 // jako argument podajemy czas, przez który ma czekać na dane

			if (this->sel.isReady(serverSocket)) // metoda isReady() sprawdza, czy dane gniazdo ma dane do odebrania
												 // jeśli do motedy isReady() przekażemy gniazdo nasłuchujące, true oznacza, że ktoś chce się do niego podłaczyć
			{
				sf::TcpSocket * tmp = new sf::TcpSocket;
				this->serverSocket.accept(*tmp); // skoro ktoś chce się do nas połączyć, to go akceptujemy
				this->clients.push_back(tmp); // i dodajemy go do listy
				this->sel.add(*tmp); // oraz do selektora, żeby można było od niego odbierać dane
									 // nie zapomnij, by usunąć(za pomocą delete) gniazdo, kiedy się rozłączy

				this->num_of_clients++;
			}
			//...
			//reszta kodu serwera
		}
	}

	//losujemy poczatkowe polozenia graczy
	for (int i = 0; i < MAX_NUM_OF_CLIENTS; i++)
	{
		Direction::point_t point;
		unsigned int player = 1;
		while (player != 0)
		{
			point = { (unsigned int)(rand() % BOARD_SIZE), (unsigned int)(rand() % BOARD_SIZE) };
			player = this->globalData.getCellValueFromBoardMatrix(point);
		}
		this->globalData.setPlayerHeadPosition(i + 1, point);
	}

	this->receivingPacketsThread.launch();
	this->processingPacketsThread.launch();
	this->sendingPacketsThread.launch();
	this->timeCountingCollisionCheckingThread.launch();
}


void Server::processingPackets()
{
	bool empty = false;
	FromClientToServerData * data = new FromClientToServerData();

	while (true)
	{
		this->mutex->lock();
		empty = this->receivedPacketsQueue.empty();
		this->mutex->unlock();

		if (!empty)
		{
			//odebranie pakietu
			this->mutex->lock();
			data = (FromClientToServerData*)this->receivedPacketsQueue.popFirst();
			this->mutex->unlock();
			//uaktualnienie globalData
			unsigned int id = data->getClientIDfromClientToServerData();
			globalData.setPlayerDirection(id, data->getNewDirection_forClient());
			cout << "gracz ID: " << id << " zmienil kierunek z " << data->getOldDirection_forClient().getDestination() <<
				 " na: " << (int)data->getNewDirection_forClient().getDestination() << endl;
			//przygotowanie pakietu do wyslania
			FromServerToClientData * sendData = new FromServerToClientData();
			*sendData = globalData;
			this->mutex->lock();
			this->sentPacketsQueue.push((BaseData*)sendData);
			this->mutex->unlock();
		}

		if (globalData.getIsDeath() == true)
		{
			FromServerToClientData * sendData = new FromServerToClientData();
			*sendData = globalData;
			this->mutex->lock();
			this->sentPacketsQueue.push((BaseData*)sendData);
			this->mutex->unlock();
		}
	}
}

void Server::receivingPackets()
{
	FromClientToServerData *receiveData = new FromClientToServerData();
	sf::Packet pack;
	while (true)
	{	// pętla główna serwera
		if (this->sel.wait(sf::seconds(2))) // jeśli metoda wait() zwróci true, to znaczy, że któreś z dodanych gniazd jest gotowe do odbioru
		{							 // jako argument podajemy czas, przez który ma czekać na dane
									 /*
									 if (this->sel.isReady(serverSocket)) // metoda isReady() sprawdza, czy dane gniazdo ma dane do odebrania
									 // jeśli do motedy isReady() przekażemy gniazdo nasłuchujące, true oznacza, że ktoś chce się do niego podłaczyć
									 {
									 sf::TcpSocket * tmp = new sf::TcpSocket;
									 this->serverSocket.accept(*tmp); // skoro ktoś chce się do nas połączyć, to go akceptujemy
									 this->clients.push_back(tmp); // i dodajemy go do listy
									 this->sel.add(*tmp); // oraz do selektora, żeby można było od niego odbierać dane
									 // nie zapomnij, by usunąć(za pomocą delete) gniazdo, kiedy się rozłączy
									 this->num_of_clients++;
									 }*/

									 // pętla przechodząca po kontenerze gniazd (zależy od typu kontenera)

			for (int i = 0; (size_t)i < this->clients.size(); i++) // u nas to jest for i indeks i
			{
				if (this->sel.isReady(*this->clients[i])) // *clients[i] coś nam wysłał
				{

					this->clients[i]->receive(pack); // 2

					receiveData->unpack(pack);

					//sf::Lock lock(this->mutex); 
					this->mutex->lock();
					this->receivedPacketsQueue.push((BaseData*)receiveData);
					this->mutex->unlock();

				}
			}
		}
	}
}

void Server::sendingPackets()
{
	//sf::Packet pack;
	InitializingData initData;
	//Nie mam pojecia jakim cudem tu tworzysz initData, ktore niezainicjowane powino miec smieci,
	//ale w czesci 'server_to_client' mialo 'PRZYPADKIEM' dokladnie takie wartosci jak global data.
	//Wiec za chwile uruchomimy setInitializingData(), ktore przypisze wartosci tylko w czesci 'initializing'
	//a potem bedziemy siegac do pol przeznaczonych dla 'server_to_klient' i magicznie beda tam akurat te wartosci o ktore nam chodzi
	//chociaz nigdy ich tam nie wpisalismy i byly tam przypadkiem od stworzenia zmiennej initData
	//W zwiazku z tym tam gdzie uzywalas initData.getPlayerHeadPosition(), ja wpisze globalData.getPlayerHeadPosition(playerNum),
	//bo w tych wywolaniach chodzi o dane ktore i tak siedza w global. (duzy cout w liniach 184-185)
	//Przez to wpisywane position nie jest {0,0}, tylko takie jakie faktycznie bylo wczesniej wylosowane

	for (int i = 0; (size_t)i < this->clients.size(); i++)
	{
		initData.setInitializingData(i + 1, this->globalData.getPlayerHeadPosition(i + 1), this->globalData.getPlayerDirection_forServer(i + 1));
		//cout << "S: Wyslano do ID: " << initData.getClientIDinitializingData() << endl << endl;
		sf::Packet pack;
		initData.pack(pack);

		if (this->clients[i]->send(pack) != sf::Socket::Done)
		{
			// nie można wysłać danych (prawdopodobnie klient/serwer się rozłączył)
			cerr << "S: Nie mozna wyslac danych!\n";
			//break;
		}
		else
		{
			unsigned int playerNum = initData.getClientIDinitializingData();
			cout << "S: Wyslano do ID: " << playerNum << " position: " << globalData.getPlayerHeadPosition(playerNum).x;
			cout << " & " << globalData.getPlayerHeadPosition(playerNum).y << " direction: " << int(globalData.getPlayerDirection_forServer(playerNum).getDestination()) << endl;
			//break;
		}
	}
	bool empty = false;
	FromServerToClientData * sendData = new FromServerToClientData(); //to be commented!!!!!!!!!!!!!!!!!!!1
	while (true)
	{
		//std::this_thread::sleep_for(std::chrono::seconds(DELAY_IN_SHOW_LOGS));
		//sf::Lock lock(this->mutex);
		this->mutex->lock();
		empty = this->sentPacketsQueue.empty();
		this->mutex->unlock();

		if (!empty)
		{
			//sf::Lock lock(this->mutex);
			this->mutex->lock();
			sendData = (FromServerToClientData*)this->sentPacketsQueue.popFirst();// front();
														 //this->sentPacketsQueue.pop();
			this->mutex->unlock();

			for (int i = 0; (size_t)i < this->clients.size(); i++) // u nas to jest for i indeks i
			{
				sf::Packet pack;
				sendData->pack(pack);
				if (this->clients[i]->send(pack) != sf::Socket::Done)
				{
					// nie można wysłać danych (prawdopodobnie klient/serwer się rozłączył)
					cerr << "S: Nie mozna wyslac danych!\n";
					//break;
				}
				else
				{
					cout << "S: Wyslano dane do ID: " << i + 1 << endl;
					//break;
				}
			}
		}
		//this->mutex.unlock();
	}
}


void Server::timeCountingCollisionChecking()
{
	const float speed = SPEED;
	sf::Clock clock;
	sf::Time t1 = clock.getElapsedTime();

	while (true)
	{
		while (t1.asSeconds() >= 1 / speed)
		{
			t1 = clock.getElapsedTime(); //waiting loop
		}
		clock.restart();
		for (int i = 0; i < clients.size(); i++)
		{
			bool isAlive = true;
			if (isAlive)
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
				if (newHeadposition.x<0 || newHeadposition.y<0 || newHeadposition.x >= BOARD_SIZE || newHeadposition.y >= BOARD_SIZE)
				{
					isAlive = false;
				}
				else
				{
					//kolizja z cialem weza:
					unsigned int matrixContent = globalData.getCellValueFromBoardMatrix(newHeadposition);
					if (matrixContent != 0)
					{
						isAlive = false;
					}
				}
				
				if (isAlive)
				{
					this->mutex->lock(); //not sure if globalData needs mutex protection
					globalData.setPlayerHeadPosition(i + 1, newHeadposition);
					globalData.setPlayerHeadOnBoard(i + 1, newHeadposition);
					this->mutex->unlock();
					cout << "head position: " <<i << ": "<< newHeadposition.x << ", " << newHeadposition.y << endl;
				}
				else
				{
					this->mutex->lock();
					globalData.setDeath(true);
					this->mutex->unlock();
					cout << "death of player: " << i << endl;
					///adekwatnie do nawiazywania polaczen z klientami -> kod ze Server::Start()
					///--sf::TcpSocket * tmp = new sf::TcpSocket;
					///--this->serverSocket.accept(*tmp); // skoro ktoś chce się do nas połączyć, to go akceptujemy
					///--this->clients.push_back(tmp); // i dodajemy go do listy
					///--this->sel.add(*tmp); // oraz do selektora, żeby można było od niego odbierać dane
					///--					 // nie zapomnij, by usunąć(za pomocą delete) gniazdo, kiedy się rozłączy
					///--this->num_of_clients++;

					//TODO_ - zapobiec wysylaniu spamu klientom i jakos elegancko zakonczyc 
					//Solution: odlaczanie klientow
					sf::TcpSocket * tmp = clients[i];
					//na cele testow nieodlaczajmy klienta bo wywala blad :(
					//this->clients[i]->disconnect(); // odlacznie klienta 
					//this->clients.erase(clients.begin() + i); // usuwanie z vektora klientow
					//this->sel.remove(*tmp); //usuwanie z selektora
					//this->num_of_clients--;
					
				}
			}
		}
		//end of for clients
	}
	return; //unreachable statement
}



Server::~Server()
{
}