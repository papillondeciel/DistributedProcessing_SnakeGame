#pragma once
#include "Direction.h"
#include "BaseData.h"


class FromServerToClientData : BaseData
{
	unsigned int ** boardMatrix;
	Direction * directionsTable;
	Direction::point_t * playerHeadPositionTable;
	bool isDeath;

public:

	FromServerToClientData();
	void operator=(const FromServerToClientData & d);

	void setPlayerHeadPosition(unsigned int playerNumber, Direction::point_t point);
	void setPlayerHeadOnBoard(unsigned int playerNumber, Direction::point_t point);
	void setPlayerDirection(unsigned int playerNumber, Direction dir);
	void setFromServerToClientData(unsigned int ** matrix, Direction * dirTab);
	void setDeath(bool death);

	bool getIsDeath();
	unsigned int getCellValueFromBoardMatrix(Direction::point_t field);
	Direction getPlayerDirection_forServer(unsigned int playerNumber);
	Direction::point_t getPlayerHeadPosition(unsigned int playerNumber);

	void writeMatrix();
	void writeDirectionsTable();
	void pack(sf::Packet & package) override;
	void unpack(sf::Packet & package) override;
};