#include "Battle.h"
#include <iostream>

using namespace std;

Battle::Battle(Player* p)
{
	player = p;
	enemy = new Player("Enemy");
	turn = 0;
}

Battle::~Battle() 
{
	delete enemy;
}

void Battle::start() 
{
	cout << "Début du combat entre " << player->getPseudo() << " et " << enemy->getPseudo() << " !" << endl;
	while(!isOver())
	{
		playTurn();
		turn ++;
	}

	Player* winner = getWinner();
	if(winner)
		cout << "Le gagnant est : " << winner->getPseudo() << endl;
	else
		cout << "Match nul !" << endl;
}

void Battle::playTurn() 
{
	for(int i = 0; i < player->getTeamSize(); i++)
	{
		Character* c = player->getTeamCharacter(i);
		if(c->getPV() > 0)
			c->applyPassives();
	}
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
			playCharacter(c);
	}
}

void Battle::playCharacter(Character* character) 
{
	Capacity* capa = character->chooseCapa();

	if(capa)
		capa->use();
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