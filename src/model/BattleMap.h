#ifndef BATTLEMAP_H
#define BATTLEMAP_H

#include "Squares.h"
#include "Player.h"

class BattleMap
{
	private:
		int sizeMap;
		int nbFree;

		Square tabBox[2][10];

	public:
		BattleMap(void);
		~BattleMap(void);

		void init(Player* p, Player* e);

		int getSize(void);
		int getNbFree(void);
		Square& getSquare(int x, int y);
};

#endif