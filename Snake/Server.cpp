#include "Server.h"
#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <thread>
#include <Windows.h>
#include <process.h>
#include "ownTypes.h"
#include "Direction.h"
#include "Data.h"

using namespace std;

Server::Server(unsigned int port) :
	num_of_clients(0),
	counter(0),
	processingPacketsThread(&Server::processingPackets, this),
	receivingPacketsThread(&Server::receivingPackets, this),
	sendingPacketsThread(&Server::sendingPackets, this)
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
			point = { (unsigned int)(rand() % BOARD_SIZE), (unsigned int)(rand() % BOARD_SIZE)};
			player = this->globalData.getCellValueFromBoardMatrix(point);

		}
		this->globalData.setPlayerHeadPosition(i, point);
	}
	
	this->receivingPacketsThread.launch();
	this->processingPacketsThread.launch();
	this->sendingPacketsThread.launch();
}


void Server::processingPackets()
{
	bool empty = false;
	Data data;

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
			data = this->receivedPacketsQueue.popFirst();// front();
			//this->receivedPacketsQueue.pop();
			this->mutex->unlock();

			/*cout << "Odebrano dane: " << endl;
			data.writeMatrix();
			cout << " & " << endl;
			data.writeDirectionsTable();*/
			//cout << "elo, S procesuje dane, i tu powinien cos wykminic" << endl;
			cout << "gracz ID: " << data.getClientIDfromClientToServerData() << " zmienil kierunek na: " << (int)data.getNewDirection() << endl;
			//" od adres: " << temp2.remoteAdressOfClient << " port:" << temp2.remotePortOfClient << endl;
		}
		//this->mutex->unlock();
	}
}

void Server::receivingPackets()
{
	Data receiveData;
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

					receiveData.unpack(pack, Data::fromCLIENT_toSERVER_enum);
					
					//sf::Lock lock(this->mutex); 
					this->mutex->lock();
					this->receivedPacketsQueue.push(receiveData);
					this->mutex->unlock();
					
				}
			}
			//...
			//reszta kodu serwera
		}
	}
}

void Server::sendingPackets()
{
	sf::Packet pack;
	Data initData;

	for (int i = 0; (size_t)i < this->clients.size(); i++)
	{
		initData.setInitializingData(i+1, this->globalData.getPlayerHeadPosition(i+1), this->globalData.getPlayerDirection(i+1));

		initData.pack(pack,Data::INITfromSERVER_enum);

		if (this->clients[i]->send(pack) != sf::Socket::Done)
		{
			// nie można wysłać danych (prawdopodobnie klient/serwer się rozłączył)
			cerr << "S: Nie mozna wyslac danych!\n";
			//break;
		}
		else
		{
			cout << "S: Wyslano do ID: " << initData.getClientIDinitializingData() << " position: " << initData.getPlayerHeadPosition(i + 1).x;
			cout << " & " << initData.getPlayerHeadPosition(i + 1).y << " direction: "  << int(initData.getPlayerDirection(i + 1).getDirection()) << endl;
			//break;
		}
	}
	bool empty = false;
	Data sendData;

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
			sendData = this->sentPacketsQueue.popFirst();// front();
			//this->sentPacketsQueue.pop();
			this->mutex->unlock();

			for (int i = 0; (size_t)i < this->clients.size(); i++) // u nas to jest for i indeks i
			{

				sendData.pack(pack, Data::fromSERVER_toCLIENT_enum);
				if (this->clients[i]->send(pack) != sf::Socket::Done)
				{
					// nie można wysłać danych (prawdopodobnie klient/serwer się rozłączył)
					cerr << "S: Nie mozna wyslac danych!\n";
					//break;
				}
				else
				{
					cout << "S: Wyslano dane do ID: " << i+1 << endl;
					//break;
				}
			}
		}
		//this->mutex.unlock();
	}
}



Server::~Server()
{
}
