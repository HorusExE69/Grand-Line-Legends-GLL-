#include "Game.h"
#include "../model/Player.h"
#include "Event.h"
#include "../view/ViewText.h"
#include "Battle.h"
#include <iostream>
#include <fstream>
#include <ctime>

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
	selectedIndex = 0;
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
		case EventType::BATTLE:
		{
			state = GameState::BATTLE;
			currentChapter->playNextTurn();
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
		case GameState::BATTLE:
			view.displayBattle();
			break;
		default:
			break;
	}
}

void Game::init(void)
{
	static bool seeded = false;
    if(!seeded) {
        srand(time(nullptr));
        seeded = true;
    }
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

// INPUT

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
		
		case GameState::BATTLE:
			return inputBattle(c);
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
		case 'r': return EventType::PLAY;
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
		default: return EventType::NONE;
	}
}

EventType Game::inputShop(char c)
{
	// En cours de développement
	return EventType::NONE;
}

EventType Game::inputBattle(char c)
{
	switch (c)
	{
		case 'r': return EventType::PLAY;
		default: return EventType::NONE;
	}
}

// SELECTION

void Game::moveSelectionUp()
{
	if(selectedIndex > 0)
			selectedIndex--;
}

void Game::moveSelectionDown()
{
	if(selectedIndex < player->getNbUnlock() - 1)
			selectedIndex++;
}

void Game::confirmSelection()
{
	Character* c = player->getUnlockCharacter(selectedIndex);
	if(c == nullptr) return;
	for(int i = 0; i < player->getTeamSize(); i++)
	{
		if(player->getTeamCharacter(i) == c)
		{
			return;
		}
	}
	player->addToTeam(c);
}

int Game::getSelectedIndex() const
{
	return selectedIndex;
}

Battle* Game::getCurrentChapter() const
{
	return currentChapter;
}