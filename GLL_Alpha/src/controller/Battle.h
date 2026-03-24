#ifndef BATTLE_H
#define BATTLE_H

#include "../model/BattleMap.h"
#include "../model/Player.h"
#include "../model/Characters.h"

struct TurnEntry
{
	Character* character;
	int speed;
};

class Battle
{
	private:
		TurnEntry** turnOrder;
		int turnOrderSize;
		Player* player;
		Player* enemy;
		int turn;
	public:
		Battle(Player* p=nullptr);
		~Battle(void);
		
		void prepareTurnOrder();
		void playAllTurns();
		bool playNextTurn();
		void playCharacter(Character* character);

		bool isDead(Player* p);
		bool isOver();
		Player* getWinner();

		Player* getPlayer() const;
		Player* getEnemy() const;

		int getTurn() const;
};

#endif