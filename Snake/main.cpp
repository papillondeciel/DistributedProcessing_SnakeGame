// Snake.cpp : Defines the entry point for the console application.
//

#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <thread> 
#include <Windows.h>
#include <process.h>
#include "Server.h"
#include "Client.h"
#include "ownTypes.h"


using namespace std;


int main()
{
	/*sf::RenderWindow window(sf::VideoMode(200, 200), "SFML works!");
	sf::CircleShape shape(100.f);
	shape.setFillColor(sf::Color::Green);

	while (window.isOpen())
	{
	sf::Event event;
	while (window.pollEvent(event))
	{
	if (event.type == sf::Event::Closed)
	window.close();
	}

	window.clear();
	window.draw(shape);
	window.display();
	}*/

	char option;
	cin >> option;

	if (option == 's')
	{
		Server server(SERVER_PORT);
		server.start();
	}
	else if (option == 'c')
	{
		unsigned int port = SERVER_PORT; // port na którym nasłuchuje serwer
		cin >> port;
		Client client(port);
		client.start();
		/*sf::TcpSocket socket; // tworzymy gniazdo klienta
		sf::IpAddress ip = "localhost";
		unsigned int port = SERVER_PORT; // port na którym nasłuchuje serwer

		cin >> port;
		cerr << " moj port " << socket.getLocalPort() << endl;
		while (true)
		{
			if (socket.connect(ip, port) != sf::Socket::Done) // łączymy się z adresem 'ip' na porcie 'port'											  // jeśli funkcja connect zwróci sf::Socket::Done oznacza to, że wszystko poszło dobrze
			{
				cerr << "C: Nie można połączyć się z " << ip.toString() << endl;
				//exit(1);
			}
			else
			{
				cerr << "C: Połączyłem się!!!" << endl;
				cerr << "C: moj port to: " << socket.getLocalPort() << endl;
				break;
			}
		}
		*/
		//data_t d;
		//cin >> d.text >> d.num;
		/*
		while (i<10)
		{
			i++;
			//waits 5 seconds
			std::this_thread::sleep_for(std::chrono::seconds(DELAY_IN_SHOW_LOGS));
			//sf::sleep(sf::milliseconds(10)); //usypianie watku

			// wysyłanie danych
			sf::Packet pak;
			d.num++;
			pak << d.text << d.num; 
			if(socket.send(pak) != sf::Socket::Done) 
			{
				// nie można wysłać danych (prawdopodobnie klient/serwer się rozłączył)
				cerr << "C: Nie można wysłać danych!\n";
				//break;
			}
			else
			{
				cout << "C: Wyslano dane: " << d.text << " & " << d.num << " do serwera, moj port " << socket.getLocalPort() << " remote port " << socket.getRemotePort() << endl;
				//break;
			}
			

		}*/
	}

	system("pause");
	return 0;
}

