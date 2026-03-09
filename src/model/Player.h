#ifndef PLAYER_H
#define PLAYER_H

#include <string>
#include "Characters.h"

class Player
{
	private:
		std::string pseudo;
		std::string lvl;

		int berries;

		Character* ownCharacter;

	public:
		Player(void);
		~Player(void);
};

#endif