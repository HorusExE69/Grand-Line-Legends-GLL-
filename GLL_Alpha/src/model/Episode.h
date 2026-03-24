#ifndef EPISODE_H
#define EPISODE_H

#include "string"
#include "Characters.h"
#include "Reward.h"

using namespace std;

class Episode
{
	public:
		std::string name;

		Character** enemies;
		Character* boss;

		Reward** rewards;

    bool isBoss;
};

#endif