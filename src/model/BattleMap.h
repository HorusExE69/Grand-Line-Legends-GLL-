#ifndef BATTLEMAP_H
#define BATTLEMAP_H

#include "Squares.h"

class BattleMap
{
	private:
		int sizeMap;
		int nbFree;

		Square** tabBox;

	public:
		BattleMap(void);
		BattleMap(int size);
		~BattleMap(void);

		int getSize(void);
		int getNbFree(void);
		Square& getSquare(int x, int y);
};

#endif