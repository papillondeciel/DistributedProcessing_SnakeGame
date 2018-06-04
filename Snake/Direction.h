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
	} direction_t;

	struct point_t
	{
		unsigned int x;
		unsigned int y;
	};

private:
	direction_t value;

public:

	Direction();

	direction_t getDirection();
	direction_t random();
	void setDirection(direction_t dir);

	operator int();
	void convertFromInt(int);

	~Direction();
};

