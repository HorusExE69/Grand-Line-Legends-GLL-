#include "ViewText.h"
#include "../controller/Game.h"
#include "../controller/Event.h"

#include <iostream>

using namespace std;

ViewText::ViewText(Game* g)
{
	game = g;
	running = false;
}

ViewText::~ViewText(void)
{
}

char ViewText::displayMenu()
{
	cout << "Bienvenue dans Grand Line Legends" << endl;
	cout << "Voulez-vous :" << endl;
	cout << "Jouer (j)" << endl;
	cout << "Acheter (a)" << endl;
	cout << "Quitter (q)" << endl;

	char c;
	cin >> c;
	return c;
}

void ViewText::handlePlay()
{
	cout << "Vous voici dans la partie combat" << endl;
	cout << "Voulez vous lancer ? (y/n)" << endl;
	char c;
	cin >> c;
	if (c == 'y') {handleBattle();}
	else {displayMenu();}
}

void ViewText::handleBattle()
{
	Event ev;
	ev.type = EventType::BATTLE;
	game->update(&ev);
}

void ViewText::handleShop()
{
	cout << "Vous voici dans le magasin" << endl;
	cout << "En cours de développement..." << endl;
}

void ViewText::handleQuit()
{
	cout << "Merci d'avoir joué." << endl;
	cout <<  "Fin du programme." << endl;
	running = false;
}

bool ViewText::isRunning() const
{
	return running;
}

void ViewText::run()
{
	running = true;
	while (running)
	{
		Event ev;

		char c = displayMenu();

		switch (c) 
		{
			case 'j':
			{
				ev.type = EventType::PLAY;
				break;
			}
			case 'a':
			{
				ev.type = EventType::SHOP;
				break;
			}
			case 'q':
			{
				ev.type = EventType::QUIT;
				running = false;
				break;
			}
			default:
			{
				cout << "Choix invalide" << endl;
				ev.type = EventType::NONE;
				break;
			}
		}

		game->update(&ev);
	}
}