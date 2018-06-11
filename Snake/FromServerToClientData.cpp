#include "FromServerToClientData.h"
#include "ownTypes.h"

using namespace std;


FromServerToClientData::FromServerToClientData()
{
	this->boardMatrix = new unsigned int *[BOARD_SIZE];
	for (int i = 0; i < BOARD_SIZE; i++)
	{
		this->boardMatrix[i] = new unsigned int[BOARD_SIZE];
		for (int x = 0; x < BOARD_SIZE; x++)
			this->boardMatrix[i][x] = 0;
	}
	this->directionsTable = new Direction[MAX_NUM_OF_CLIENTS];
	this->playerHeadPositionTable = new Direction::point_t[MAX_NUM_OF_CLIENTS];
	for (int i = 0; i < MAX_NUM_OF_CLIENTS; i++)
	{
		this->playerHeadPositionTable[i] = { 0,0 };
	}
	this->isDeath = false;
}

void FromServerToClientData::operator=(const FromServerToClientData & d)
{
	this->boardMatrix = d.boardMatrix;
	this->directionsTable = d.directionsTable;
	this->isDeath = d.isDeath;
}

void FromServerToClientData::setPlayerHeadPosition(unsigned int playerNumber, Direction::point_t point)
{
	this->playerHeadPositionTable[playerNumber - 1] = point;
	//this->boardMatrix[point.x][point.y] = playerNumber;
}
//newstuff
void FromServerToClientData::setPlayerHeadOnBoard(unsigned int playerNumber, Direction::point_t point)
{
	this->boardMatrix[point.x][point.y] = playerNumber;
}
void FromServerToClientData::setPlayerDirection(unsigned int playerNumber, Direction dir)
{
	directionsTable[playerNumber - 1] = dir;
}
void FromServerToClientData::setDeath(bool death)
{
	this->isDeath = death;
}
bool FromServerToClientData::getIsDeath()
{
	return isDeath;
}
//end of newstuff
void FromServerToClientData::setFromServerToClientData(unsigned int ** matrix, Direction * dirTab)
{
	this->boardMatrix = matrix;
	this->directionsTable = dirTab;
}

unsigned int FromServerToClientData::getCellValueFromBoardMatrix(Direction::point_t field)
{
	unsigned int result;

	result = this->boardMatrix[field.x][field.y];

	return result;
}

Direction FromServerToClientData::getPlayerDirection_forServer(unsigned int playerNumber)
{
	Direction result;
	result = this->directionsTable[playerNumber - 1];
	return result;
}

Direction::point_t FromServerToClientData::getPlayerHeadPosition(unsigned int playerNumber)
{
	Direction::point_t result;

	result = this->playerHeadPositionTable[playerNumber - 1];

	return result;
}


void FromServerToClientData::writeMatrix()
{
	for (int i = 0; i < BOARD_SIZE; i++)
	{
		for (int x = 0; x < BOARD_SIZE; x++)
		{
			cout << this->boardMatrix[i][x] << " ";
		}
		cout << endl;
	}
}


void FromServerToClientData::writeDirectionsTable()
{

	for (int x = 0; x < BOARD_SIZE; x++)
	{
		//nie wiem czy tutaj sie dobrze wypisuje
		//na pewno jest to kwestia enuma
		cout << this->directionsTable[x].getDestination() << " ";
	}
	cout << endl;
}


void FromServerToClientData::pack(sf::Packet & package)
{
	for (int i = 0; i < BOARD_SIZE; i++)
	{
		for (int x = 0; x < BOARD_SIZE; x++)
		{
			package << this->boardMatrix[i][x];
		}
	}
	for (int i = 0; i < MAX_NUM_OF_CLIENTS; i++)
	{
		package << (int)(this->directionsTable[i].getDestination());
	}
	package << isDeath;
}

void FromServerToClientData::unpack(sf::Packet & package)
{
	int tempNumber;
	for (int i = 0; i < BOARD_SIZE; i++)
	{
		for (int x = 0; x < BOARD_SIZE; x++)
		{
			package >> this->boardMatrix[i][x];
		}
	}
	for (int i = 0; i < MAX_NUM_OF_CLIENTS; i++)
	{
		package >> tempNumber;
		this->directionsTable[i].convertFromInt(tempNumber);
	}
	package >> isDeath;

}