#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>

class BaseData
{

	virtual void pack(sf::Packet & package) abstract;
	virtual void unpack(sf::Packet & package) abstract;

};