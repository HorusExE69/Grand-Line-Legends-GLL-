#include "Game.h"
#include "../model/Player.h"
#include "Event.h"
#include "../view/ViewText.h"
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
	view = ViewText(this);
}

Game::~Game(void)
{
	if (player != nullptr)
	{
		delete player;
		player = nullptr;
	}
}

void Game::update(Event* ev)
{
	switch(ev->type)
	{
		case EventType::PLAY:
		{
			view.handlePlay();
			break;
		}
		case EventType::BATTLE:
		{
			Battle* battle = new Battle(player);
			battle->start();
			break;
		}
		case EventType::SHOP:
		{
			view.handleShop();
			break;
		}
		case EventType::QUIT:
		{
			view.handleQuit();
			break;
		}
		case EventType::NONE:
		{
			break;
		}
		default:
			break;
	}
}

void Game::init(void)
{
	view.run();
}

Player* Game::getPlayer(void)
{
	return player;
}