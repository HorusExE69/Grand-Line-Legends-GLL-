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
		GameState state;

	public:
		Game(void);
		~Game(void);

		void update(Event* ev);
		void display();

		void init(void);
		Player* getPlayer(void);
		GameState getState();

		EventType input(char c);
		EventType inputMenu(char c);
		EventType inputBattlePrepa(char c);
		EventType inputShop(char c);
		EventType inputTeam(char c);
		EventType inputTeamChange(char c);
};

#endif