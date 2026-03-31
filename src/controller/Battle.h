#ifndef BATTLE_H
#define BATTLE_H

#include "../model/BattleMap.h"
#include "../model/Player.h"
#include "../model/Characters.h"

class Battle
{
	private:
		BattleMap* map;
		Player* player;
		Player* enemy;
		int turn;
	public:
		Battle(Player* p=nullptr);
		~Battle(void);

		void init();
		
		void playAllTurns();
		void playTurn();
		bool playNextTurn();
		void playCharacter(Square* character, bool side);

		bool isDead(Player* p);
		bool isOver();
		Player* getWinner();

		Player* getPlayer() const;
		Player* getEnemy() const;

		Square** getOrder(int &outSize);

		int getTurn() const;
};

#endif