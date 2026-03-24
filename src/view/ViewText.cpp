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

void ViewText::displayTeam(Player* enemy)
{
	termClear();
	cout << "Gérer l'équipe..." << endl;
	cout << "Retour (r)" << endl;
	cout << "Gérer votre équipe (g)" << endl;
	cout << "\n <<< EQUIPE ENNEMIE (" << enemy->getTeamSize() << "/" << enemy->getTeamMax() << ") >>> \n" << endl;
	showTeam(enemy);
	cout << "\n\n <<< VOTRE EQUIPE (" << game->getPlayer()->getTeamSize() << "/" << game->getPlayer()->getTeamMax() << ") >>> \n" << endl;
	showTeam(game->getPlayer());
}

void ViewText::displayTeamChange()
{
	termClear();
	
	Player* p = game->getPlayer();

	cout << "Gérer votre équipe..." << endl;
	cout << "Retour (r)" << endl;
	cout << "Utilisez ↑ ↓ pour naviguer et Entrée pour sélectionner" << endl;

	cout << "\n <<< VOTRE EQUIPE ("
		<< p->getTeamSize() << "/"
		<< p->getTeamMax() << ") >>> \n" << endl;

	for(int i = 0; i < p->getTeamSize(); i++)
	{
		Character* c = p->getTeamCharacter(i);
		if(c==nullptr) continue;
		cout << "  " << c->getName()
			<< " - PV: " << c->getPV() << endl;
	}

	cout << "\n <<< PERSONNAGES DISPONIBLES >>> \n" << endl;

	for(int i = 0; i < p->getNbUnlock(); i++)
	{
		Character* c = p->getUnlockCharacter(i);

		if(i == game->getSelectedIndex())
			cout << " > ";
		else
			cout << "   ";
		
		cout << i << " - "
			<< c->getName()
			<< " (PV: " << c->getPV() << ")"
			<< endl;
	}
}

void ViewText::displayBattle() 
{
	termClear();
	cout << "Combat entre " << game->getCurrentChapter()->getPlayer()->getPseudo() << " et " << game->getCurrentChapter()->getEnemy()->getPseudo() << " !" << endl;
	if(game->getCurrentChapter()->isOver())
	{
		cout << "\n   <<< GAGNANT >>>\n   " << game->getCurrentChapter()->getWinner()->getPseudo() << endl;
	}
	else
	{
		cout << "Tour " << game->getCurrentChapter()->getTurn() << endl;
		showTeam(game->getCurrentChapter()->getPlayer());
		showTeam(game->getCurrentChapter()->getEnemy());
	}	
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

		if(game->getState() == GameState::TEAM_CHANGE)
		{
			if(c==65)
			{
				game->moveSelectionUp();
				game->display();
				continue;
			}
			else if(c==66)
			{
				game->moveSelectionDown();
				game->display();
				continue;
			}
			else if(c==68)
			{
				game->confirmSelection();
				game->display();
				continue;
			}
			else if(c==67)
				continue;
		}
		if(game->getState() == GameState::BATTLE)
		{
			if(!game->getCurrentChapter()->isOver())
			{
				game->getCurrentChapter()->playNextTurn();
				game->display();
			}
		}

		cout.flush();
		
		Event ev;
		ev.type = game->input(c);

		game->update(&ev);

	}
}

// Fonctions d'affichage


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