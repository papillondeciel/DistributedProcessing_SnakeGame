#include "Data.h"
#include "ownTypes.h"

using namespace std;

Data::Data()
{
	//initialization struct "fromServerToClient" 
	this->fromServerToClientData.boardMatrix = new unsigned int *[BOARD_SIZE];
	for (int i = 0; i < BOARD_SIZE; i++)
	{
		this->fromServerToClientData.boardMatrix[i] = new unsigned int[BOARD_SIZE];
		for (int x = 0; x < BOARD_SIZE; x++)
			this->fromServerToClientData.boardMatrix[i][x] = 0;
	}

	this->fromServerToClientData.directionsTable = new Direction[MAX_NUM_OF_CLIENTS];
	this->fromServerToClientData.playerHeadPositionTable = new Direction::point_t[MAX_NUM_OF_CLIENTS];
	for (int i = 0; i < MAX_NUM_OF_CLIENTS; i++)
	{
		this->fromServerToClientData.playerHeadPositionTable[i] = { 0,0 };
	}

	//initialization struct "initializationData"
	///setting null values to control data, in properly running this segment of code is unuseful,
	///because before sending initializeMessage to Client, Server must set specialized values for each Client
	this->initializingData.clientID = 0;
	this->initializingData.position = {0,0};

	//initialization struct "fromClientToServerData"
	///setting null values to control data, in properly running this segment of code is unuseful,
	///because before sending messageToServer Client must set specialized values depends on its own state
	this->fromClientToServerData.clientID = 0;
	this->fromClientToServerData.positionBeforeTurn = {0,0};

}

void Data::operator=(const Data & d)
{
	this->fromServerToClientData.boardMatrix = d.fromServerToClientData.boardMatrix;
	this->fromServerToClientData.directionsTable = d.fromServerToClientData.directionsTable;
}


//Settery--------------------------------------
void Data::setPlayerHeadPosition(unsigned int playerNumber, Direction::point_t point)
{
	this->fromServerToClientData.playerHeadPositionTable[playerNumber -1] = point;
	this->fromServerToClientData.boardMatrix[point.x][point.y] = playerNumber;
}

void Data::setFromServerToClientData(unsigned int ** matrix, Direction * dirTab)
{
	this->fromServerToClientData.boardMatrix = matrix;
	this->fromServerToClientData.directionsTable = dirTab;
}

void Data::setInitializingData(unsigned int cID, Direction::point_t pos, Direction dir)
{
	this->initializingData.clientID = cID;
	this->initializingData.position = pos;
	this->initializingData.randomDirection = dir;
}

void Data::setFromClientToServerData(unsigned int cID, Direction::point_t pos, Direction oldDir, Direction newDir)
{
	this->fromClientToServerData.clientID = cID;
	this->fromClientToServerData.positionBeforeTurn = pos;
	this->fromClientToServerData.oldDirection = oldDir;
	this->fromClientToServerData.newDirection = newDir;
}

//Gettery--------------------------------------------
unsigned int Data::getCellValueFromBoardMatrix(Direction::point_t field)
{
	unsigned int result;

	result = this->fromServerToClientData.boardMatrix[field.x][field.y];

	return result;
}

Direction::point_t Data::getPlayerHeadPosition(unsigned int playerNumber)
{
	Direction::point_t result;

	result = this->fromServerToClientData.playerHeadPositionTable[playerNumber-1];

	return result;
}

unsigned int Data::getClientIDinitializingData()
{
	return this->fromClientToServerData.clientID;
}

unsigned int Data::getClientIDfromClientToServerData()
{
	return this->initializingData.clientID;
}

Direction Data::getNewDirection()
{
	return this->fromClientToServerData.newDirection;
}

Direction Data::getPlayerDirection(unsigned int playerNumber)
{
	Direction result;
	result = this->fromServerToClientData.directionsTable[playerNumber - 1];
	return result;
}






void Data::writeMatrix()
{
	for (int i = 0; i < BOARD_SIZE; i++)
	{
		for (int x = 0; x < BOARD_SIZE; x++)
		{
			cout << this->fromServerToClientData.boardMatrix[i][x] << " ";
		}
		cout << endl;
	}
}

void Data::writeDirectionsTable()
{
	
	for (int x = 0; x < BOARD_SIZE; x++)
	{
		cout << this->fromServerToClientData.directionsTable[x] << " ";
	}
	cout << endl;
}

void Data::pack(sf::Packet & package, enum Data::dataType type)
{
	switch (type)
	{
	case Data::fromSERVER_toCLIENT_enum:
		for (int i = 0; i < BOARD_SIZE; i++)
		{
			for (int x = 0; x < BOARD_SIZE; x++)
			{
				package << this->fromServerToClientData.boardMatrix[i][x];
			}
		}
		for (int i = 0; i < MAX_NUM_OF_CLIENTS; i++)
		{
			package << (int)(this->fromServerToClientData.directionsTable[i]);
		}
		break;
	case Data::INITfromSERVER_enum:
		package << this->initializingData.clientID;
		package << this->initializingData.position.x;
		package << this->initializingData.position.y;
		package << (int)(this->initializingData.randomDirection);
		break;
	case Data::fromCLIENT_toSERVER_enum:
		package << this->fromClientToServerData.clientID;
		package << this->fromClientToServerData.positionBeforeTurn.x;
		package << this->fromClientToServerData.positionBeforeTurn.y;
		package << (int)(this->fromClientToServerData.oldDirection);
		package << (int)(this->fromClientToServerData.newDirection);
		break;
	default:
		break;
	}
	
}

void Data::unpack(sf::Packet & package, enum Data::dataType type)
{
	int tempNumber;
	switch (type)
	{
	case Data::fromSERVER_toCLIENT_enum:
		for (int i = 0; i < BOARD_SIZE; i++)
		{
			for (int x = 0; x < BOARD_SIZE; x++)
			{
				package >> this->fromServerToClientData.boardMatrix[i][x];
			}
		}
		
		for (int i = 0; i < MAX_NUM_OF_CLIENTS; i++)
		{
			package >> tempNumber;
			this->fromServerToClientData.directionsTable[i].convertFromInt(tempNumber);
		}
		break;
	case Data::INITfromSERVER_enum:
		package >> this->initializingData.clientID;
		package >> this->initializingData.position.x;
		package >> this->initializingData.position.y;
		package >> tempNumber;
		this->initializingData.randomDirection.convertFromInt(tempNumber);
		break;
	case Data::fromCLIENT_toSERVER_enum:
		package >> this->fromClientToServerData.clientID;
		package >> this->fromClientToServerData.positionBeforeTurn.x;
		package >> this->fromClientToServerData.positionBeforeTurn.y;
		package >> tempNumber;
		this->fromClientToServerData.oldDirection.convertFromInt(tempNumber);
		package >> tempNumber;
		this->fromClientToServerData.newDirection.convertFromInt(tempNumber);
		break;
	default:
		break;
	}
	
}



Data::~Data()
{
}
