#pragma once
#include "Direction.h"
#include "BaseData.h"


class FromClientToServerData : BaseData
{
	unsigned int clientID;
	//Direction::point_t positionBeforeTurn;
	Direction oldDirection;
	Direction newDirection;

public:

	FromClientToServerData();

	void setFromClientToServerData(unsigned int cID, Direction::point_t pos, Direction oldDir, Direction newDir);
	unsigned int getClientIDfromClientToServerData();
	Direction getNewDirection_forClient();
	Direction getOldDirection_forClient();

	void pack(sf::Packet & package) override;
	void unpack(sf::Packet & package) override;
};