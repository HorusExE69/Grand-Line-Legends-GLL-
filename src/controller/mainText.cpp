/**
 * @file mainText.cpp
 * @brief Point d'entrée de la version textuelle (terminal) de Grand Line Legends.
 *
 * Compile avec : make text
 * Exécute avec : ./bin/GLL_text
 */

#include "Game.h"
#include "../view/ViewText.h"

int main()
{
    Game     game;
    ViewText view(&game);
    game.init();
    view.run();
    return 0;
}
