#include "InitializingData.h"
#include "ownTypes.h"

using namespace std;

	InitializingData::InitializingData()
	{
		this->clientID = 0;
		this->randomDirection.position = { 0,0 };
	}

	void InitializingData::setInitializingData(unsigned int cID, Direction::point_t pos, Direction dir)
	{
		this->clientID = cID;
		this->randomDirection.position = pos;
		this->randomDirection = dir;
	}

	unsigned int InitializingData::getClientIDinitializingData()
	{
		return this->clientID;
	}


	Direction InitializingData::getInitDirection_forClient()
	{
		return this->randomDirection;
	}


	void InitializingData::pack(sf::Packet & package)
	{
		package << this->clientID;
		package << this->randomDirection.position.x;
		package << this->randomDirection.position.y;
		package << (int)(this->randomDirection.getDestination());
	}

	void InitializingData::unpack(sf::Packet & package)
	{
		int tempNumber;
		package >> this->clientID;
		package >> this->randomDirection.position.x;
		package >> this->randomDirection.position.y;
		package >> tempNumber;
		this->randomDirection.convertFromInt(tempNumber);
	}