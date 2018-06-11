#include "Queue.h"
#include "BaseData.h"


Queue::Queue()
{
}

BaseData* Queue::popFirst()
{
	BaseData* temp;// = new DataBase();
	temp = q.front();
	q.pop();
	return temp;
}

bool Queue::empty()
{
	bool result;
	result = q.empty();
	return result;
}

void Queue::push(BaseData *d)
{
	q.push(d);
}


Queue::~Queue()
{
}