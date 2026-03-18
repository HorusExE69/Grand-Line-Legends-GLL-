#ifndef EVENT_H
#define EVENT_H

#include "../model/Utils.h"

class Event
{
	public:
		EventType type;
	public:
		Event() : type(EventType::NONE) {}
};

#endif