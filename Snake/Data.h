#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include "Direction.h"

class Data
{
	struct {
		unsigned int ** boardMatrix;
		Direction * directionsTable;
		Direction::point_t * playerHeadPositionTable;

	}fromServerToClientData;

	struct {
		unsigned int clientID;
		Direction::point_t position;
		Direction randomDirection;

	}initializingData;

	struct {
		unsigned int clientID;
		Direction::point_t positionBeforeTurn;
		Direction oldDirection;
		Direction newDirection;
	}fromClientToServerData;

public:
	enum dataType {
		fromSERVER_toCLIENT_enum,
		INITfromSERVER_enum,
		fromCLIENT_toSERVER_enum
	};

	Data();

	void operator =(const Data & );
	
	//Settery--------------------------------------
	void setPlayerHeadPosition(unsigned int playerNumber, Direction::point_t point);

	void setFromServerToClientData(unsigned int ** matrix, Direction * dirTab);
	void setInitializingData(unsigned int cID, Direction::point_t pos, Direction dir);
	void setFromClientToServerData(unsigned int cID, Direction::point_t pos, Direction oldDir, Direction newDir);

	//Gettery--------------------------------------------
	//fromServerToClientData--------------
	unsigned int getCellValueFromBoardMatrix(Direction::point_t field);
	Direction getPlayerDirection(unsigned int playerNumber);
	Direction::point_t getPlayerHeadPosition(unsigned int playerNumber);
	//initializingData--------------------
	unsigned int getClientIDinitializingData();
	unsigned int getClientIDfromClientToServerData();
	//Direction::point_t getPosition();
	Direction getNewDirection();


	void writeMatrix();
	void writeDirectionsTable();
	void pack(sf::Packet &, enum Data::dataType);
	void unpack(sf::Packet &, enum Data::dataType);


	~Data();
};

