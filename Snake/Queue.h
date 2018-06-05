#pragma once
#include <queue>
#include "Data.h"
#include "ownTypes.h"

using namespace std;


class Queue
{
	queue<Data*> q;
public:
	Queue();
	Data * popFirst();
	bool empty();
	void push(Data *);
	~Queue();
};

