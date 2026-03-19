#include "ViewText.h"
#include "../controller/Game.h"
#include "../controller/Event.h"
#include "../model/Player.h"

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
	if (c == 'y') {handleBattlePrepa();}
	else {displayMenu();}
}

void ViewText::handleBattlePrepa()
{
	cout << "Préparation du combat..." << endl;
	cout << "Lancer le combat (c)" << endl;
	cout << "Gérer l'équipe (e)" << endl;
	cout << "Retour au menu (m)" << endl;

	char c;
	cin >> c;
	switch (c)
	{
		case 'c':
		{
			handleBattle();
			break;
		}
		case 'e':
		{
			handleTeam();
			break;
		}
		case 'm':
		{
			displayMenu();
			break;
		}
	}
}

void ViewText::handleTeam()
{
	Event ev;
	ev.type = EventType::TEAM;
	game->update(&ev);
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

// Fonctions d'affichage

void displayBattleStart(Battle* battle) 
{
	cout << "Début du combat entre " << battle->getPlayer()->getPseudo() << " et " << battle->getEnemy()->getPseudo() << " !" << endl;
}

void displayBattleWinner(Battle* battle) 
{
	Player* winner = battle->getWinner();
	if(winner)
		cout << "Le gagnant est : " << winner->getPseudo() << "\n" << endl;
	else
		cout << "Match nul !\n" << endl;
}


void displayCSVError(const std::string& filepath) 
{
	cout << "Erreur : impossible d'ouvrir " << filepath << endl;
}

void showCapa(Character* c)
{
	if(c->getNbCapa() == 0 || c->getTabCapa() == nullptr)
	{
		cout << c->getName() << " n'a aucune capacite.\n";
		return;
	}

	std::cout << "Capacites de " << c->getName() << " :\n";

	for(int i = 0; i < c->getNbCapa(); i++)
	{
		Capacity* capa = c->getTabCapa()[i];
		TypeC type1 = capa->getType().t1;
		TypeC type2 = capa->getType().t2;

		cout << "- " << c->getName();

		cout << " | Type1: " << typeCToString(type1);

		cout << " | Type2: " << typeCToString(type2);

		cout << " | Chance: " << capa->getPercentage() << "%";

		cout << endl;
	}
}

void showBank(Player* p)
{
	cout << "Bank (" << p->getNbBank() << "/" << p->getBankMax() << "):" << endl;
	for (int i = 0; i < p->getNbBank(); i++)
	{
		Character* c = p->getBankCharacter(i);
		cout << "  " << RarityTostring(c->rarC()) << " - " << c->getName() << " - PV: " << c->getPV() << endl;
	}
}

void showUnlocked(Player* p)
{
	cout << "Unlocked (" << p->getNbUnlock() << "/" << p->getUlkMax() << "):" << endl;
	for (int i = 0; i < p->getNbUnlock(); i++)
	{
		Character* c = p->getUnlockCharacter(i);
		cout << "  " << Type_Tostring(c->typeC()) << " - " << c->getName() << " - PV: " << c->getPV() << endl;
	}
}

void showTeam(Player* p)
{
	cout << "Equipe active (" << p->getTeamSize() << "/" << p->getTeamMax() << "):" << endl;
	for (int i = 0; i < p->getTeamSize(); i++)
	{
		Character* c = p->getTeamCharacter(i);
		if(c == nullptr) continue;
		cout << "  " << c->getName() << " - PV: " << c->getPV();
		cout << endl;
	}
}

string getPseudoSTDIN() 
{
	cout << "Pseudo : ";
	string pseudo;
	cin >> pseudo;
	return pseudo;
}