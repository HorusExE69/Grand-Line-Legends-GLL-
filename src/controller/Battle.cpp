#include "Battle.h"
#include <iostream>
#include <unistd.h>
#include <fstream>

using namespace std;

Battle::Battle(Player* p)
{
	ifstream file("./data/Characters.csv");
	// if (!file.is_open())
	// {
	// 	displayCSVError("../data/Characters.csv");
	// }
	player = p;
	enemy = new Player(file, "./data/capacities/","Enemy");
	enemy->unlockAll();
	turn = 0;
	map = new BattleMap;
}

Battle::~Battle() 
{
	delete enemy;
	delete map;
}

void Battle::init()
{
	map->init(player, enemy);
}

void Battle::playAllTurns() 
{
	while(!isOver())
	{
		playTurn();
		turn++;
	}
}

bool Battle::playNextTurn() 
{
	if(isOver())
		return false;
	playTurn();
	turn++;
	return true;
}

void Battle::playTurn()
{
	int orderSize;
	Square** order = getOrder(orderSize);

	// Appliquer passives
	for(int i = 0; i < player->getTeamSize(); i++)
	{
		Character* c = player->getTeamCharacter(i);
		if(c->getPV() > 0)
			c->applyPassives();
	}
	for(int i = 0; i < enemy->getTeamSize(); i++)
	{
		Character* c = enemy->getTeamCharacter(i);
		if(c->getPV() > 0)
			c->applyPassives();
	}

	// Jouer les personnages vivants
	for(int i = 0; i < orderSize; i++)
	{
		bool side = order[i]->x == 0;
		playCharacter(order[i], side);
	}

	delete[] order;
}


void Battle::playCharacter(Square* character, bool side)
{
	if(!character || !character->inmate)
		return;

	Capacity* capa = character->inmate->chooseCapa();
	
	if(capa)
	{
		capa->launcher = character;

		int targetSide = side ? 1 : 0;
		int size = side ? enemy->getTeamSize() : player->getTeamSize();

		capa->addTarget(&map->getSquare(targetSide, rand() % size));

		capa->use();
	}
}


bool Battle::isDead(Player* p) 
{
	for(int i = 0; i < p->getTeamSize(); i++)
	{
		if(p->getTeamCharacter(i)->getPV() > 0)
			return false;
	}
	return true;
}

bool Battle::isOver() 
{
	return isDead(player) || isDead(enemy);
}

// Getters

Player* Battle::getWinner() 
{
	if(isDead(player) && isDead(enemy))
		return nullptr;
	else if(isDead(player))
		return enemy;
	else if(isDead(enemy))
		return player;
	else
		return nullptr;
}

Square** Battle::getOrder(int &outSize)
{
	int maxN = player->getTeamSize() + enemy->getTeamSize();
	Square** tabOrder = new Square*[maxN];
	int k = 0;

	for(int i = 0; i < player->getTeamSize(); i++)
	{
		if(player->getTeamCharacter(i)->getPV() > 0)
			tabOrder[k++] = &map->getSquare(0, i);
	}

	for(int j = 0; j < enemy->getTeamSize(); j++)
	{
		if(enemy->getTeamCharacter(j)->getPV() > 0)
			tabOrder[k++] = &map->getSquare(1, j);
	}

	for(int i = 0; i < k-1; i++)
	{
		for(int j = 0; j < k-i-1; j++)
		{
			if(tabOrder[j]->inmate && tabOrder[j+1]->inmate)
			{
				if(tabOrder[j]->inmate->speed > tabOrder[j+1]->inmate->speed)
				{
					Square* temp = tabOrder[j];
					tabOrder[j] = tabOrder[j+1];
					tabOrder[j+1] = temp;
				}
			}
		}
	}

	outSize = k;
	return tabOrder;
}


Player* Battle::getPlayer() const { return player; }
Player* Battle::getEnemy() const { return enemy; }

int Battle::getTurn() const { return turn; }