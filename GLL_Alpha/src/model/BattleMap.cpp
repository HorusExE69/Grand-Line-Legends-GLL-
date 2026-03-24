#include "Squares.h"
#include "BattleMap.h"
#include <iostream>

using namespace std;

BattleMap::BattleMap(void)
{
	sizeMap = 0;
	nbFree = 0;
	tabBox = nullptr;
}

BattleMap::BattleMap(int size)
{
	sizeMap = size;
	nbFree = size * size;
	tabBox = new Square*[sizeMap];
	for (int i = 0; i < sizeMap; i++)
	{
		tabBox[i] = new Square[sizeMap];
	}
}

BattleMap::~BattleMap(void)
{
	if(tabBox != nullptr)
	{
		delete[] tabBox;
		tabBox = nullptr;
	}
}

int BattleMap::getSize(void)
{
	return sizeMap;
}

int BattleMap::getNbFree(void)
{
	return nbFree;
}

Square& BattleMap::getSquare(int x, int y)
{
	return tabBox[x][y];
}