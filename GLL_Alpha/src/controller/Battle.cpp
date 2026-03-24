#include "Battle.h"
#include <iostream>
#include <fstream>
#include <algorithm>

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
}

Battle::~Battle() 
{
	if(enemy != nullptr) {
		delete enemy;
		enemy = nullptr;
	}

	if(turnOrder != nullptr) {
		for(int i = 0; i < turnOrderSize; i++) {
			if(turnOrder[i] != nullptr) {
				delete turnOrder[i];
				turnOrder[i] = nullptr;
			}
		}
		delete[] turnOrder;
		turnOrder = nullptr;
		turnOrderSize = 0;
	}
}

void Battle::prepareTurnOrder() 
{
	int count = 0;
	for (int i = 0; i < player->getTeamSize(); i++)
		if (Character* c = player->getTeamCharacter(i))
			if (c->getPV() > 0) count++;

	for (int i = 0; i < enemy->getTeamSize(); i++)
		if (Character* c = enemy->getTeamCharacter(i))
			if (c->getPV() > 0) count++;

	int oldSize = turnOrderSize;
	turnOrderSize = count;

	if (turnOrder != nullptr) {
		for (int i = 0; i < oldSize; i++) {
			delete turnOrder[i];
		}
		delete[] turnOrder;
		turnOrder = nullptr;
	}

	if (turnOrderSize == 0) return;
	turnOrder = new TurnEntry*[turnOrderSize];

	int index = 0;

	for (int i = 0; i < player->getTeamSize(); i++) {
		Character* c = player->getTeamCharacter(i);
		if (c && c->getPV() > 0) {
			turnOrder[index++] = new TurnEntry{c, c->getSpeed()};
		}
	}

	for (int i = 0; i < enemy->getTeamSize(); i++) {
		Character* c = enemy->getTeamCharacter(i);
		if (c && c->getPV() > 0) {
			turnOrder[index++] = new TurnEntry{c, c->getSpeed()};
		}
	}

	if (index != turnOrderSize) {
		std::cerr << "Erreur: mismatch entre turnOrderSize et index réel!" << std::endl;
		turnOrderSize = index;
	}

	std::sort(turnOrder, turnOrder + turnOrderSize, [](TurnEntry* a, TurnEntry* b) {
		return a->speed > b->speed;
	});
}

void Battle::playAllTurns() 
{
	while(!isOver())
	{
		playNextTurn();
		turn++;
	}
}

bool Battle::playNextTurn() 
{
	if(isOver())
		return false;

	prepareTurnOrder();

	for(int i = 0; i < turnOrderSize; i++)
	{
		Character* c = turnOrder[i]->character;

		if(c->getPV() <= 0) 
			continue;

		c->applyPassives();
		playCharacter(c);
	}

	turn++;
	return true;
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

Player* Battle::getPlayer() const { return player; }
Player* Battle::getEnemy() const { return enemy; }

int Battle::getTurn() const { return turn; }