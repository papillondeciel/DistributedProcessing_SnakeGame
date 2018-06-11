#include "Direction.h"
#include <cstdlib>


Direction::Direction()
{
	this->value = this->randomDestination();
}

Direction::destination_t Direction::getDestination()
{
	return this->value;
}

Direction::destination_t Direction::randomDestination()
{
	destination_t result;
	result = destination_t(rand() % 4 + 1);
	return result;
}

void Direction::setDestination(destination_t dir)
{
	this->value = dir;
}

//Direction::operator int()
//{
//	return int(value);
//}

void Direction::convertFromInt(int integer)
{
	this->value = destination_t(integer);
}

Direction::~Direction()
{
}