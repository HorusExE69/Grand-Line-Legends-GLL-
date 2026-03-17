#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include "../model/Player.h"
#include "../model/Characters.h"
#include "../model/BattleMap.h"
#include "Game.h"


using namespace std;


void showMapTxt(BattleMap& map)
{
	for (int i = 0; i < map.getSize(); i++)
	{
		for (int j = 0; j < map.getSize(); j++)
		{
			if (map.getSquare(i, j).isEmpty())
			{
				cout << "0 ";
			}
			else
			{
				cout << "X ";
			}
		}
		cout << endl;
	}
}

int main()
{
	Game game;
	return(0);
}