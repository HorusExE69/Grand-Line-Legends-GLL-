#ifndef ARC_H
#define ARC_H

#include "Episode.h"
#include "Characters.h"
#include <string>

class Arc
{
	public:
		std::string name;

		Episode** episodes;

		Character* unlockCharacter;
		float unlockChance; // 1.0 = 100%, 0.5 = 50%
};

#endif