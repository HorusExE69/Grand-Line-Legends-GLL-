#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include "../model/Player.h"
#include "../model/Characters.h"
#include "../model/BattleMap.h"
#include "../view/ViewGraph.h"
#include "Game.h"

int main()
{
    Game game;

    ViewGraph view(&game);

    view.displayBattle();

    return 0;
}