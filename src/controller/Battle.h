#ifndef BATTLE_H
#define BATTLE_H

#include "../model/BattleMap.h"
#include "../model/Player.h"
#include "../model/Characters.h"

class Battle
{
	private:
		Player* player;
		Player* enemy;
		int turn;
	public:
		Battle(Player* p=nullptr);
		~Battle(void);
		
		void start();
		void playTurn();
		void playCharacter(Character* character);

		bool isDead(Player* p);
		bool isOver();
		Player* getWinner();
};

#endif