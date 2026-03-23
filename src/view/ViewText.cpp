#include "ViewText.h"
#include "../controller/Game.h"
#include "../controller/Event.h"
#include "../model/Player.h"
#include "winTxt.h"

#include <iostream>

using namespace std;

ViewText::ViewText(Game* g) : win(80, 24)
{
	game = g;
	running = false;
}

ViewText::~ViewText(void)
{
}

void ViewText::displayMenu()
{
	termClear();
	cout << "=======< Grand Line Legends >=======" << endl;
	cout << "Bienvenue dans Grand Line Legends" << endl;
	cout << "Voulez-vous :" << endl;
	cout << "Jouer (j)" << endl;
	cout << "Acheter (a)" << endl;
	cout << "Quitter (q)" << endl;
}

void ViewText::displayPlay()
{
	termClear();
	cout << "Préparation du combat..." << endl;
	cout << "Lancer le combat (c)" << endl;
	cout << "Gérer l'équipe (e)" << endl;
	cout << "Retour au menu (m)" << endl;
}

void ViewText::handleShop()
{
	termClear();
	cout << "Vous voici dans le magasin" << endl;
	cout << "En cours de développement..." << endl;
}

void ViewText::handleQuit()
{
	termClear();
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
	cout.flush();
	running = true;
	while (running)
	{
		char c = win.getCh();
		if (c == '\0') continue;
		
		cout.flush();
		
		Event ev;
		ev.type = game->input(c);

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
	string pseudo;
	cout << "Entrez votre pseudo : ";
	cin >> pseudo;
	cout << "\nBienvenue, " << pseudo << " !" << endl;
	cout << "Appuyez sur Entrée pour continuer..." << endl;
	cin.ignore();
	cin.get();
	termClear();
	return pseudo;
}



