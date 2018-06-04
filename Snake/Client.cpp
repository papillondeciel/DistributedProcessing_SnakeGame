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
#include "Data.h"



using namespace std;


Client::Client(unsigned int port) :
	init(false),
	myID(0),
	processingReceivedPacketsThread(&Client::processingReceivedPackets, this),
	receivingPacketsThread(&Client::receivingPackets, this),
	sendingPacketsThread(&Client::sendingPackets, this),
	processingPlayerInteractionsThread(&Client::processingPlayerInteractions, this)
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
}


void Client::processingReceivedPackets()
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
			cout << "C procesuje dane ---> i tu kminie rzeczy" << endl;
			if (this->myID==0)
			{
				cout << "C: odebralem info inicjalizyjace" << endl;
				///odbieram paczke inizjalizujaca
				this->myID = data.getClientIDinitializingData();
				cout << "C: moje ID: " << this->myID << endl;
			}
			else
			{
				cout << "C: odebralem info regularne :D "<< endl;
				///odbieram paczki z ogolnym info o stanie gry :D

			}
			//" od adres: " << temp2.remoteAdressOfClient << " port:" << temp2.remotePortOfClient << endl;
		}
		//this->mutex->unlock();
	}
}

void Client::receivingPackets()
{
	Data receiveData;
	sf::Packet pack;
	while (true)
	{

		if (this->socket.receive(pack)==sf::Socket::Done)
		{
			if (!init)
			{
				receiveData.unpack(pack, Data::INITfromSERVER_enum);
				init = true;
			}
			else
				receiveData.unpack(pack, Data::fromSERVER_toCLIENT_enum);

			//sf::Lock lock(this->mutex); 
			this->mutex->lock();
			this->receivedPacketsQueue.push(receiveData);
			this->mutex->unlock();
		}		
	}
}

void Client::sendingPackets()
{
	sf::Packet pack;
	Data sendData;
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
			sendData = this->sentPacketsQueue.popFirst();// front();
			//this->sentPacketsQueue.pop();
			this->mutex->unlock();

			sendData.pack(pack, Data::fromCLIENT_toSERVER_enum);
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
	/*int c;
	while (true)
	{
		c = 0;
		switch ((c = _getch())) {
		case KEY_UP:
			cout << endl << "Up" << endl;//key up
			break;
		case KEY_DOWN:
			cout << endl << "Down" << endl;   // key down
			break;
		case KEY_LEFT:
			cout << endl << "Left" << endl;  // key left
			break;
		case KEY_RIGHT:
			cout << endl << "Right" << endl;  // key right
			break;
		default:
			cout << endl << "null" << endl;  // not arrow
			break;
		}

	}*/
	HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
	DWORD NumInputs = 0;
	DWORD InputsRead = 0;
	//bool running = true;

	INPUT_RECORD irInput;

	Direction::direction_t direct;
	bool setNewDirect = false;
	Data tempData;
	Direction tempDir;

	GetNumberOfConsoleInputEvents(hInput, &NumInputs);
	
	while (true)
	{
		ReadConsoleInput(hInput, &irInput, 1, &InputsRead);
		if (irInput.Event.KeyEvent.bKeyDown)
		{
			switch (irInput.Event.KeyEvent.wVirtualKeyCode)
			{
			case VK_ESCAPE:
				puts("Escape");
				break;

			case VK_LEFT:
				puts("Left");
				direct = Direction::direction_t::LEFT;
				setNewDirect = true;
				break;

			case VK_UP:
				puts("Up");
				direct = Direction::direction_t::UP;
				setNewDirect = true;
				break;

			case VK_RIGHT:
				puts("Right");
				direct = Direction::direction_t::RIGHT;
				setNewDirect = true;
				break;

			case VK_DOWN:
				puts("Down");
				direct = Direction::direction_t::DOWN;
				setNewDirect = true;
				break;
			}
			if (setNewDirect)
			{
				tempDir.setDirection(direct);
				tempData.setFromClientToServerData(this->myID, { 0,0 }, tempDir, tempDir);
				this->mutex->lock();
				this->sentPacketsQueue.push(tempData);
				this->mutex->unlock();
				setNewDirect = false;
			}

		}
	}
}



Client::~Client()
{
}
