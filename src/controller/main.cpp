/**
 * @file main.cpp
 * @brief Point d'entrée de la version graphique SDL2 de Grand Line Legends.
 *
 * Compile avec : make
 * Exécute avec : ./bin/GLL
 */

#include "Game.h"
#include "../view/ViewGraph.h"

int main()
{
    Game      game;
    ViewGraph view(&game);
    view.run();
    return 0;
}
