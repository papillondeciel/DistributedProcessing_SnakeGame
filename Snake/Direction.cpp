#include "Direction.h"
#include <cstdlib>


Direction::Direction()
{
	this->value = this->random();
}

Direction::direction_t Direction::getDirection()
{
	return this->value;
}

Direction::direction_t Direction::random()
{
	direction_t result;
	result = direction_t(rand() % 4 + 1);
	return result;
}

void Direction::setDirection(direction_t dir)
{
	this->value = dir;
}

Direction::operator int()
{
	return int(value);
}

void Direction::convertFromInt(int integer)
{
	this->value = direction_t(integer);
}

Direction::~Direction()
{
}
