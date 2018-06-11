#pragma once
#include "Direction.h"
#include "BaseData.h"

class InitializingData : BaseData
{
	unsigned int clientID;
	//Direction::point_t position;
	Direction randomDirection;

public:

	InitializingData();

	void setInitializingData(unsigned int cID, Direction::point_t pos, Direction dir);
	unsigned int getClientIDinitializingData();
	Direction getInitDirection_forClient();

	void pack(sf::Packet & package) override;
	void unpack(sf::Packet & package) override;

};