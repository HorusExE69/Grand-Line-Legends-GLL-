#ifndef VIEWTEXT_H
#define VIEWTEXT_H

#include <iostream>
#include <string>
#include "../controller/Battle.h"
#include "../controller/Shop.h"
#include "winTxt.h"


class Game;

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

		void displayMenu();
		void displayPlay();
		void displayTeam(Player* enemy);
		void displayTeamChange();
		void displayBattle();
		void handleShop();
		void handleQuit();
};

#endif