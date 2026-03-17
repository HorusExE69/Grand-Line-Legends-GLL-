#ifndef GAME_H
#define GAME_H

#include "../model/Player.h"

class Game
{
	private:
		Player* player;
		int currentArc;
		int currentChapter;

	public:
		Game(void);
		~Game(void);

		void init(void);
		Player* getPlayer(void);
};

#endif