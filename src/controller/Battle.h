#ifndef BATTLE_H
#define BATTLE_H

#include "../model/BattleMap.h"
#include "../model/Player.h"

class Battle
{
	private:
		BattleMap* map;
		Player* player;
		int turn;
	public:
		Battle(Player* player=nullptr);
		~Battle(void);
		
		void start(void);
		void end(void);
};

#endif