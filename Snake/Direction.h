#pragma once
class Direction
{
public:
	typedef enum
	{
		//UNKNOW = 0,
		UP = 1,
		RIGHT = 2,
		DOWN = 3,
		LEFT = 4
	} destination_t;

	struct point_t
	{
		int x;
		int y;
	}position;

private:
	destination_t value;

public:

	Direction();

	destination_t getDestination();
	destination_t randomDestination();
	void setDestination(destination_t dir);


	//operator int();
	void convertFromInt(int);

	~Direction();
};
