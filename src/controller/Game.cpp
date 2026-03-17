#include "Game.h"
#include "../model/Player.h"
#include "Battle.h"
#include <iostream>
#include <fstream>

using namespace std;

Game::Game(void)
{
	ifstream file("./data/Characters.csv");
	if (!file.is_open())
	{
		cout << "Erreur : impossible d'ouvrir ../data/Characters.csv" << endl;
	}
	cout << "Pseudo : ";
	string pseudo;
	cin >> pseudo;
	player = new Player(file, "./data/capacities/", pseudo);
	file.close();
	currentArc = 0;
	currentChapter = 0;
}

Game::~Game(void)
{
	if (player != nullptr)
	{
		delete player;
		player = nullptr;
	}
}

void Game::init(void)
{
	Battle b = Battle(player);
	b.start();
}

Player* Game::getPlayer(void)
{
	return player;
}