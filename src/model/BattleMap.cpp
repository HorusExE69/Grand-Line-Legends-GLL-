#include "Squares.h"
#include "BattleMap.h"
#include "Player.h"
#include <iostream>

using namespace std;

BattleMap::BattleMap(void)
{
	sizeMap = 0;
	nbFree = 0;
	for(int i = 0; i<2; i++)
	{
		for(int j = 0; j < 10; j++)
		{
			tabBox[i][j] = Square(i,j);
		}
	}
}

BattleMap::~BattleMap(void)
{
}

void BattleMap::init(Player* p, Player* e)
{
	sizeMap = p->getTeamMax() + e->getTeamMax();
	nbFree = sizeMap - p->getTeamSize() - e->getTeamSize();
	for (int i = 0; i < 10; i++)
	{
		if(i < p->getTeamSize())
			tabBox[0][i].hold(p->getTeamCharacter(i));
	}
	for (int i = 0; i < 10; i++)
	{
		if(i < e->getTeamSize())
			tabBox[1][i].hold(e->getTeamCharacter(i));
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