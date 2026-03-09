#ifndef BATTLEMAP_H
#define BATTLEMAP_H

#include "Squares.h"

class BattleMap
{
	private:
		int sizeMap;
		int nbFree;

		Square** tabBox;
};

#endif