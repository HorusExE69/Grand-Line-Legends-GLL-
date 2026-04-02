#include "ViewGraph.h"
#include "../controller/Game.h"
#include <iostream>

ViewGraph::ViewGraph(Game* g)
    : window(nullptr), renderer(nullptr), font(nullptr), game(g)
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cerr << "Erreur SDL : " << SDL_GetError() << std::endl;
        return;
    }

    if (TTF_Init() == -1)
    {
        std::cerr << "Erreur TTF : " << TTF_GetError() << std::endl;
        return;
    }

    window = SDL_CreateWindow(
        "GLL Graphique",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        900,
        600,
        SDL_WINDOW_SHOWN
    );

    if (!window)
    {
        std::cerr << "Erreur fenêtre SDL\n";
        return;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if (!renderer)
    {
        std::cerr << "Erreur renderer SDL\n";
        return;
    }

    font = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 24);

    if (!font)
    {
        std::cerr << "Erreur chargement police : " << TTF_GetError() << std::endl;
    }
}

ViewGraph::~ViewGraph()
{
    if (font) TTF_CloseFont(font);
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);

    TTF_Quit();
    SDL_Quit();
}

void ViewGraph::renderText(const std::string& text, int x, int y)
{
    if (!font) return;

    SDL_Color color = {255,255,255,255};

    SDL_Surface* surface = TTF_RenderText_Blended(font, text.c_str(), color);

    if (!surface) return;

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_Rect rect;
    rect.x = x;
    rect.y = y;
    rect.w = surface->w;
    rect.h = surface->h;

    SDL_FreeSurface(surface);

    SDL_RenderCopy(renderer, texture, NULL, &rect);

    SDL_DestroyTexture(texture);
}

void ViewGraph::displayBattle()
{
    bool running = true;
    SDL_Event event;

    while (running)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
                running = false;
        }

        SDL_SetRenderDrawColor(renderer, 30,30,30,255);
        SDL_RenderClear(renderer);

        renderText("Mode Combat",50,50);
        renderText("Equipe Joueur :",50,120);
        renderText("Personnage 1",80,160);
        renderText("Personnage 2",80,200);

        renderText("Ennemis :",500,120);
        renderText("Ennemi 1",530,160);
        renderText("Ennemi 2",530,200);

        SDL_RenderPresent(renderer);

        SDL_Delay(16);
    }
}

void ViewGraph::displayTeam()
{
    bool running = true;
    SDL_Event event;

    while (running)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
                running = false;
        }

        SDL_SetRenderDrawColor(renderer, 20,20,60,255);
        SDL_RenderClear(renderer);

        renderText("Equipe du Joueur",50,50);

        renderText("Personnage 1",80,150);
        renderText("Personnage 2",80,200);
        renderText("Personnage 3",80,250);
        renderText("Personnage 4",80,300);

        SDL_RenderPresent(renderer);

        SDL_Delay(16);
    }
}