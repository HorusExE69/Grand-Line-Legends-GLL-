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
	string pseudo = "User";
	player = new Player(file, "./data/capacities/", pseudo);
	file.close();
	currentArc = 0;
	currentChapter = new Battle(player);
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
		case EventType::MENU:
		{
			state = GameState::MENU;
			break;
		}
		case EventType::PLAY:
		{
			state = GameState::BATTLE_PREPA;
			break;
		}
		case EventType::TEAM:
		{
			state = GameState::TEAM;
			break;
		}
		case EventType::TEAM_CHANGE:
		{
			state = GameState::TEAM_CHANGE;
			break;
		}
		case EventType::ADD_TO_TEAM:
		{
			player->addToTeam(player->getUnlockCharacter(view.handleATT()));
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
	display();
}

void Game::display()
{
	switch(state)
	{
		case GameState::MENU:
			view.displayMenu();
			break;
		case GameState::BATTLE_PREPA:
			view.displayPlay();
			break;
		case GameState::TEAM:
			view.displayTeam(currentChapter->getEnemy());
			break;
		case GameState::TEAM_CHANGE:
			view.displayTeamChange();
			break;
		default:
			break;
	}
}

void Game::init(void)
{
	currentChapter->getEnemy()->addTeamSize(4);
	currentChapter->getEnemy()->randomTeam();
	string pseudo = getPseudoSTDIN();
	player->changePseudo(pseudo);
	player->unlockAll();
	player->addTeamSize(4);
	state = GameState::MENU;
	view = ViewText(this);
	display();
	view.run();
}

Player* Game::getPlayer(void)
{
	return player;
}

GameState Game::getState()
{
	return state;
}

EventType Game::input(char c)
{
	switch(state)
	{
		case GameState::MENU:
			return inputMenu(c);

		case GameState::BATTLE_PREPA:
			return inputBattlePrepa(c);

		case GameState::SHOP:
			return inputShop(c);

		case GameState::TEAM:
			 return inputTeam(c);

		case GameState::TEAM_CHANGE:
			 return inputTeamChange(c);
		default:
			break;
	}
	return EventType::NONE;
}

EventType Game::inputMenu(char c)
{
	switch (c)
	{
		case 'j': return EventType::PLAY;
		case 'a': return EventType::SHOP;
		case 'q': return EventType::QUIT;
		default: return EventType::NONE;
	}
}

EventType Game::inputBattlePrepa(char c)
{
	switch (c)
	{
		case 'c': return EventType::BATTLE;
		case 'e': return EventType::TEAM;
		case 'm': return EventType::MENU;
		default: return EventType::NONE;
	}
}

EventType Game::inputTeam(char c)
{
	switch (c)
	{
		case 'm': return EventType::PLAY;
		case 'e': return EventType::PLAY;
		case 'g': return EventType::TEAM_CHANGE;
		default: return EventType::NONE;
	}
}

EventType Game::inputTeamChange(char c)
{
	switch (c)
	{
		case 'r': return EventType::TEAM;
		case 'a': return EventType::ADD_TO_TEAM;
		default: return EventType::NONE;
	}
}

EventType Game::inputShop(char c)
{
	// En cours de développement
	return EventType::NONE;
}