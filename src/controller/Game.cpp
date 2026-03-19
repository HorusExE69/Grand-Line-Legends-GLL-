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
		displayCSVError("../data/Characters.csv");
	}
	string pseudo = getPseudoSTDIN();
	player = new Player(file, "./data/capacities/", pseudo);
	file.close();
	currentArc = 0;
	currentChapter = new Battle(player);
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
		case EventType::BATTLE_PREPA:
		{
			view.handleBattlePrepa();
			break;
		}
		case EventType::TEAM:
		{
			currentChapter->getEnemy()->addTeamSize(4);
			currentChapter->getEnemy()->randomTeam();
			showTeam(currentChapter->getEnemy());
			break;
		}
		case EventType::BATTLE:
		{
			displayBattleStart(currentChapter);
			currentChapter->start();
			displayBattleWinner(currentChapter);
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
	player->unlockAll();
	view.run();
}

Player* Game::getPlayer(void)
{
	return player;
}