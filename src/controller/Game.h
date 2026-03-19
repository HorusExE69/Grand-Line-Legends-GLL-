#ifndef GAME_H
#define GAME_H

#include "../model/Player.h"
#include "Event.h"
#include "../view/ViewText.h"


class Game
{
	private:
		Player* player;
		int currentArc;
		Battle* currentChapter;
		ViewText view;

	public:
		Game(void);
		~Game(void);

		void update(Event* ev);

		void init(void);
		Player* getPlayer(void);
};

#endif