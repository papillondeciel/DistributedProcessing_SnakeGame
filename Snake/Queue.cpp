#include "Queue.h"
#include "Data.h"


Queue::Queue()
{
}

Data *Queue::popFirst()
{
	Data *temp = new Data();
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

void Queue::push(Data *d)
{
	q.push(d);
}


Queue::~Queue()
{
}
