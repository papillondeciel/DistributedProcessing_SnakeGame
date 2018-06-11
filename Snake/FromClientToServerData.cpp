#include "FromClientToServerData.h"
#include "ownTypes.h"

using namespace std;


FromClientToServerData::FromClientToServerData()
{
	this->clientID = 0;
}


void FromClientToServerData::setFromClientToServerData(unsigned int cID, Direction::point_t pos, Direction oldDir, Direction newDir)
{
	this->clientID = cID;
	//this->fromClientToServerData.positionBeforeTurn = pos;
	this->oldDirection = oldDir;
	this->newDirection = newDir;
}


unsigned int FromClientToServerData::getClientIDfromClientToServerData()
{
	return this->clientID;
}

Direction FromClientToServerData::getNewDirection_forClient()
{
	return this->newDirection;
}

Direction FromClientToServerData::getOldDirection_forClient()
{
	return this->oldDirection;
}



void FromClientToServerData::pack(sf::Packet & package)
{

	package << this->clientID;
	package << this->oldDirection.position.x;
	package << this->oldDirection.position.y;
	package << (int)(this->oldDirection.getDestination());
	package << this->newDirection.position.x;
	package << this->newDirection.position.y;
	package << (int)(this->newDirection.getDestination());
}

void FromClientToServerData::unpack(sf::Packet & package)
{
	int tempNumber;
	package >> this->clientID;
	package >> this->oldDirection.position.x;
	package >> this->oldDirection.position.y;
	package >> tempNumber;
	this->oldDirection.convertFromInt(tempNumber);
	package >> this->newDirection.position.x;
	package >> this->newDirection.position.y;
	package >> tempNumber;
	this->newDirection.convertFromInt(tempNumber);
}