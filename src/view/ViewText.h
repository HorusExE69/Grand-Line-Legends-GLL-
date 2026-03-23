#ifndef VIEWTEXT_H
#define VIEWTEXT_H

#include <iostream>
#include <string>
#include "../controller/Battle.h"
#include "../controller/Shop.h"
#include "winTxt.h"


class Game;

string readInput(WinTXT& win);
void displayBattleStart(Battle* battle);
void displayBattleWinner(Battle* battle);
void displayCSVError(const std::string& filepath);
void showCapa(Character* c);
void showBank(Player* p);
void showUnlocked(Player* p);
void showTeam(Player* p);
string getPseudoSTDIN();

class ViewText
{
	private:
		Game* game;
		bool running;
		WinTXT win;

	public:
		ViewText(Game* g = nullptr);
		~ViewText(void);
		bool isRunning() const;
		void run();

		int handleATT();
		void displayMenu();
		void displayPlay();
		void displayTeam(Player* enemy);
		void displayTeamChange();
		void handleShop();
		void handleQuit();
};

#endif // VIEWTEXT_H