#ifndef VIEWTEXT_H
#define VIEWTEXT_H

#include <iostream>
#include "../controller/Battle.h"
#include "../controller/Shop.h"


class Game;

class ViewText
{
	private:
		Game* game;
		bool running;

	public:
		ViewText(Game* g = nullptr);
		~ViewText(void);
		bool isRunning() const;
		void run();

		char displayMenu();
		void handlePlay();
		void handleBattle();
		void handleShop();
		void handleQuit();
};

#endif // VIEWTEXT_H