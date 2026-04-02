#ifndef VIEWGRAPH_H
#define VIEWGRAPH_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>

class Game;

class ViewGraph {

public:
    ViewGraph(Game* g);
    ~ViewGraph();

    void displayBattle();
    void displayTeam();

private:
    void renderText(const std::string& text, int x, int y);

    SDL_Window* window;
    SDL_Renderer* renderer;
    TTF_Font* font;

    Game* game;
};

#endif