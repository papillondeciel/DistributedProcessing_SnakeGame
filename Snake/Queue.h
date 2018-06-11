#pragma once
#include <queue>
#include "BaseData.h"
#include "ownTypes.h"

using namespace std;


class Queue
{
	queue<BaseData*> q;
public:
	Queue();
	BaseData * popFirst();
	bool empty();
	void push(BaseData *);
	~Queue();
};