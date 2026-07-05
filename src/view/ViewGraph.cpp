/**
 * @file ViewGraph.cpp
 * @brief Implémentation de la vue graphique SDL2.
 *
 * Corrections appliquées :
 *  - Fenêtre à taille fixe (SDL_RenderSetLogicalSize pour HiDPI/Wayland)
 *  - Overlay de luminosité (applyBrightnessOverlay)
 *  - Support souris complet (handleMouseClick)
 *  - Affichage des messages de feedback (renderStatusMsg)
 *  - Libellés complets (Type_DisplayString, Rarity_DisplayString)
 *  - Shop : uniquement des coffres
 *  - Campagne : arcs/épisodes verrouillés affichés
 *  - Combat : affichage grille numérotée + récompenses
 *  - Bank détail : scroll des capacités à la molette
 */

#include "ViewGraph.h"
#include "../controller/Game.h"
#include "../controller/Event.h"
#include "../controller/Battle.h"
#include "../model/Campaign.h"
#include "../model/Settings.h"
#include "../model/Shop.h"
#include "../model/SaveData.h"
#include <SDL2/SDL_image.h>
#include <cstdio>
#include <string>

using namespace std;

static SDL_Window* g_window = nullptr; // set during init

// Chemins police
static const char* FONT_PATHS[] = {
    "assets/fonts/arial.ttf",                              // projet (fonctionne partout)
    "C:/Windows/Fonts/arial.ttf",                          // Windows
    "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",     // Linux
    "/System/Library/Fonts/Helvetica.ttc",                 // macOS
    nullptr
};

static TTF_Font* loadFont(int size)
{
    for (int i = 0; FONT_PATHS[i] != nullptr; i++)
    {
        TTF_Font* f = TTF_OpenFont(FONT_PATHS[i], size);
        if (f) return f;
    }
    return nullptr;
}

// Constructeur / Destructeur

ViewGraph::ViewGraph(Game* g)
    : window(nullptr), renderer(nullptr),
      font(nullptr), fontSm(nullptr), game(g),
      rewardApplied(false), capaScrollOffset(0),
      bankScrollStart(0), mouseX(0), mouseY(0),
      displayedMsg(""), statusMsgExpiry(0), selectedTeamSlot(-1),
      bgTex(nullptr), lastBattle(nullptr), battleRewardMsg("")
{

    if (SDL_Init(SDL_INIT_VIDEO) < 0)  return;
    if (TTF_Init() == -1)              return;
    IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);

    window = SDL_CreateWindow("Grand Line Legends",
                              SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              SDL_WIN_W, SDL_WIN_H,
                              SDL_WINDOW_SHOWN);
    if (!window) return;
    g_window = window;

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) return;

    // Résolution logique fixe (utile sur écrans HiDPI / Wayland)
    SDL_RenderSetLogicalSize(renderer, SDL_WIN_W, SDL_WIN_H);

    font   = loadFont(22);
    fontSm = loadFont(16);

    // Chargement du fond One Piece (bg.jpg en priorité, sinon bg.png)
    {
        SDL_Surface* s = IMG_Load("assets/bg.jpg");
        if (!s) s = IMG_Load("assets/bg.png");
        if (s) {
            bgTex = SDL_CreateTextureFromSurface(renderer, s);
            SDL_FreeSurface(s);
        }
    }

    // Les portraits par personnage sont chargés à la demande dans renderCharacterSprite()
}

ViewGraph::~ViewGraph()
{
    if (bgTex) SDL_DestroyTexture(bgTex);
    for (auto& kv : charTex)
        if (kv.second) SDL_DestroyTexture(kv.second);
    if (fontSm)   TTF_CloseFont(fontSm);
    if (font)     TTF_CloseFont(font);
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window)   SDL_DestroyWindow(window);
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
}

// Primitives

void ViewGraph::renderText(const string& text, int x, int y, SDL_Color col)
{
    if (!font || text.empty()) return;
    SDL_Surface* surf = TTF_RenderUTF8_Blended(font, text.c_str(), col);
    if (!surf) return;
    SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
    SDL_Rect     dst = {x, y, surf->w, surf->h};
    SDL_FreeSurface(surf);
    SDL_RenderCopy(renderer, tex, nullptr, &dst);
    SDL_DestroyTexture(tex);
}

void ViewGraph::renderTextSm(const string& text, int x, int y, SDL_Color col)
{
    if (!fontSm || text.empty()) return;
    SDL_Surface* surf = TTF_RenderUTF8_Blended(fontSm, text.c_str(), col);
    if (!surf) return;
    SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
    SDL_Rect     dst = {x, y, surf->w, surf->h};
    SDL_FreeSurface(surf);
    SDL_RenderCopy(renderer, tex, nullptr, &dst);
    SDL_DestroyTexture(tex);
}

void ViewGraph::renderTextCentered(const string& text, int y, SDL_Color col)
{
    if (!font || text.empty()) return;
    int tw = 0, th = 0;
    TTF_SizeUTF8(font, text.c_str(), &tw, &th);
    renderText(text, (SDL_WIN_W - tw) / 2, y, col);
}

void ViewGraph::renderRect(int x, int y, int w, int h, SDL_Color col)
{
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, col.r, col.g, col.b, col.a);
    SDL_Rect r = {x, y, w, h};
    SDL_RenderFillRect(renderer, &r);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
}

void ViewGraph::renderOutlineRect(int x, int y, int w, int h, SDL_Color col)
{
    SDL_SetRenderDrawColor(renderer, col.r, col.g, col.b, col.a);
    SDL_Rect r = {x, y, w, h};
    SDL_RenderDrawRect(renderer, &r);
}

void ViewGraph::renderLine(int x1, int y1, int x2, int y2, SDL_Color col)
{
    SDL_SetRenderDrawColor(renderer, col.r, col.g, col.b, col.a);
    SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
}

void ViewGraph::clear(SDL_Color bg)
{
    SDL_SetRenderDrawColor(renderer, bg.r, bg.g, bg.b, bg.a);
    SDL_RenderClear(renderer);
}

void ViewGraph::applyBrightnessOverlay()
{
    int brightness = game->getSettings()->getBrightness();
    if (brightness >= 100) return;
    Uint8 alpha = static_cast<Uint8>((100 - brightness) * 255 / 100);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, alpha);
    SDL_RenderFillRect(renderer, nullptr);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
}

void ViewGraph::renderStatusMsg()
{
    // Récupérer un nouveau message du jeu et démarrer le timer
    string newMsg = game->getStatusMsg();
    if (!newMsg.empty()) {
        displayedMsg    = newMsg;
        statusMsgExpiry = SDL_GetTicks() + 3000; // visible 3 secondes
        game->clearStatusMsg();
    }
    // Afficher tant que le timer n'est pas expiré
    if (displayedMsg.empty() || SDL_GetTicks() > statusMsgExpiry) {
        displayedMsg = "";
        return;
    }
    renderRect(60, 724, 904, 36, {20, 20, 60, 220});
    renderOutlineRect(60, 724, 904, 36, {100, 100, 200, 255});
    renderTextSm(">> " + displayedMsg, 80, 732, {255, 255, 120, 255});
}

bool ViewGraph::hovered(int bx, int by, int bw, int bh) const
{
    return mouseX >= bx && mouseX <= bx + bw &&
           mouseY >= by && mouseY <= by + bh;
}

void ViewGraph::renderButton(const string& label, int x, int y, int w, int h,
                              SDL_Color base, SDL_Color hov)
{
    SDL_Color col = hovered(x, y, w, h) ? hov : base;
    renderRect(x, y, w, h, col);
    if (hovered(x, y, w, h))
        renderOutlineRect(x, y, w, h, {255, 255, 255, 160});
    renderTextSm(label, x + 10, y + (h - 16) / 2, {255, 255, 255, 255});
}

void ViewGraph::present()
{
    SDL_RenderPresent(renderer);
}

// Helpers

void ViewGraph::fireInput(char c)
{
    GameState prevState = game->getState();
    Event ev(game->input(c));
    game->update(&ev);
    GameState newState  = game->getState();
    // Réinitialiser rewardApplied et capaScrollOffset lors de transitions
    if (prevState != GameState::BATTLE && newState == GameState::BATTLE)
        rewardApplied = false;
    // Purger lastBattle quand on quitte BATTLE pour éviter un pointeur dangling
    // (startBattle() supprime l'ancien objet Battle via delete currentBattle)
    if (prevState == GameState::BATTLE && newState != GameState::BATTLE)
        lastBattle = nullptr;
    if (newState != GameState::BANK_DETAIL)
        capaScrollOffset = 0;
    // Réinitialiser le slot sélectionné quand on quitte TEAM_CHANGE
    if (prevState == GameState::TEAM_CHANGE && newState != GameState::TEAM_CHANGE)
        selectedTeamSlot = -1;
}

// Fond One Piece

void ViewGraph::renderOnePieceBg()
{
    // Utiliser la texture BMP générée si disponible
    if (bgTex) {
        int imgW = 0, imgH = 0;
        SDL_QueryTexture(bgTex, nullptr, nullptr, &imgW, &imgH);
        SDL_Rect dst = {0, 0, SDL_WIN_W, SDL_WIN_H};
        SDL_Rect src;
        if (imgW > 0 && imgH > 0) {
            float canvasAR = (float)SDL_WIN_W / SDL_WIN_H;
            float imgAR    = (float)imgW / imgH;
            if (imgAR > canvasAR) {
                int srcW = (int)(imgH * canvasAR);
                src = {(imgW - srcW) / 2, 0, srcW, imgH};
            } else {
                int srcH = (int)(imgW / canvasAR);
                src = {0, (imgH - srcH) / 2, imgW, srcH};
            }
            SDL_RenderCopy(renderer, bgTex, &src, &dst);
        } else {
            SDL_RenderCopy(renderer, bgTex, nullptr, &dst);
        }
        return;
    }

    // Ciel : dégradé nuit profonde → aube orange à l'horizon (y=0 → y=420)
    for (int y = 0; y <= 420; y++)
    {
        float t = static_cast<float>(y) / 420.0f;
        Uint8 r = static_cast<Uint8>(  5 + t * 180);
        Uint8 g = static_cast<Uint8>(  5 + t *  80);
        Uint8 b = static_cast<Uint8>( 30 + t *  20);
        SDL_SetRenderDrawColor(renderer, r, g, b, 255);
        SDL_RenderDrawLine(renderer, 0, y, SDL_WIN_W, y);
    }
    // Océan : bleu profond → reflets dorés à l'horizon (y=420 → y=768)
    for (int y = 420; y < SDL_WIN_H; y++)
    {
        float t = static_cast<float>(y - 420) / (SDL_WIN_H - 420);
        Uint8 r = static_cast<Uint8>( 10 + t *  30);
        Uint8 g = static_cast<Uint8>( 30 + t *  60);
        Uint8 b = static_cast<Uint8>( 80 + t *  60);
        SDL_SetRenderDrawColor(renderer, r, g, b, 255);
        SDL_RenderDrawLine(renderer, 0, y, SDL_WIN_W, y);
    }

    // // Soleil levant
    // Rayons (lignes partant du centre vers l'extérieur)
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    int sx = 512, sy = 422; // centre horizon
    for (int a = 0; a < 16; a++)
    {
        float angle = static_cast<float>(a) * 3.14159f / 8.0f;
        int   ex    = sx + static_cast<int>(300.0f * SDL_sinf(angle));
        int   ey    = sy - static_cast<int>(300.0f * SDL_fabsf(SDL_cosf(angle)));
        SDL_SetRenderDrawColor(renderer, 255, 200, 50, 30);
        for (int w = -6; w <= 6; w++)
            SDL_RenderDrawLine(renderer, sx + w, sy, ex + w, ey);
    }
    // Disque solaire (couches concentriques)
    static const struct { int r; Uint8 R, G, B, A; } SUN_LAYERS[] = {
        {90, 255, 240, 100, 50}, {70, 255, 220,  60, 100},
        {50, 255, 190,  20, 180},{30, 255, 160,   0, 230},
        {15, 255, 230, 120, 255},{0,0,0,0,0}
    };
    for (int i = 0; SUN_LAYERS[i].r > 0; i++)
    {
        int r = SUN_LAYERS[i].r;
        SDL_SetRenderDrawColor(renderer, SUN_LAYERS[i].R, SUN_LAYERS[i].G,
                               SUN_LAYERS[i].B, SUN_LAYERS[i].A);
        SDL_Rect rect = {sx - r, sy - r, r * 2, r * 2};
        SDL_RenderFillRect(renderer, &rect);
    }

    // // Reflet du soleil sur l'eau
    for (int row = 0; row < 18; row++)
    {
        int   ry = 425 + row * 19;
        int   hw = 40 + row * 8;
        Uint8 al = static_cast<Uint8>(60 - row * 3);
        SDL_SetRenderDrawColor(renderer, 255, 180, 50, al);
        SDL_Rect rr = {sx - hw, ry, hw * 2, 8};
        SDL_RenderFillRect(renderer, &rr);
    }

    // // Vagues
    for (int w = 0; w < 6; w++)
    {
        int wy = 460 + w * 50;
        SDL_SetRenderDrawColor(renderer, 40 + w * 10, 100 + w * 8, 160, 80 + w * 15);
        for (int x = 0; x < SDL_WIN_W; x += 80)
        {
            // Petite crête de vague (arc approximé)
            for (int dx = 0; dx < 80; dx++)
            {
                int dy = static_cast<int>(8.0f * SDL_sinf(static_cast<float>(dx) * 3.14159f / 80.0f));
                SDL_RenderDrawPoint(renderer, x + dx, wy - dy);
                SDL_RenderDrawPoint(renderer, x + dx, wy - dy + 1);
            }
        }
    }

    // // Étoiles dans le ciel
    static const int STAR_POS[][2] = {
        {80,40},{200,20},{350,60},{470,15},{600,35},{720,55},{850,25},{950,45},
        {130,90},{280,80},{420,100},{550,70},{680,95},{800,85},{100,130},{450,140}
    };
    for (int i = 0; i < 16; i++)
    {
        Uint8 br = 150 + (i * 7) % 105;
        SDL_SetRenderDrawColor(renderer, br, br, br, 200);
        SDL_RenderDrawPoint(renderer, STAR_POS[i][0], STAR_POS[i][1]);
        SDL_RenderDrawPoint(renderer, STAR_POS[i][0]+1, STAR_POS[i][1]);
        SDL_RenderDrawPoint(renderer, STAR_POS[i][0], STAR_POS[i][1]+1);
    }

    // // Jolly Roger stylisé (haut gauche)
    // Crâne : carré blanc
    renderRect(30, 20, 50, 40, {200, 200, 200, 160});
    // Orbites : deux petits carrés noirs
    renderRect(38, 28, 10, 10, {20, 20, 20, 200});
    renderRect(60, 28, 10, 10, {20, 20, 20, 200});
    // Machoire : barre horizontale
    renderRect(36, 48, 38, 6,  {20, 20, 20, 200});
    // Os croisés : deux rectangles en X
    SDL_SetRenderDrawColor(renderer, 200, 200, 200, 140);
    SDL_RenderDrawLine(renderer, 20, 65, 90, 80);
    SDL_RenderDrawLine(renderer, 90, 65, 20, 80);
    for (int t = -2; t <= 2; t++) {
        SDL_RenderDrawLine(renderer, 20+t, 65, 90+t, 80);
        SDL_RenderDrawLine(renderer, 90+t, 65, 20+t, 80);
    }

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
}

// Fond dégradé

void ViewGraph::renderGradientBg(SDL_Color top, SDL_Color bot)
{
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    for (int y = 0; y < SDL_WIN_H; y++)
    {
        float t = static_cast<float>(y) / SDL_WIN_H;
        Uint8 r = static_cast<Uint8>(top.r + t * (bot.r - top.r));
        Uint8 g = static_cast<Uint8>(top.g + t * (bot.g - top.g));
        Uint8 b = static_cast<Uint8>(top.b + t * (bot.b - top.b));
        SDL_SetRenderDrawColor(renderer, r, g, b, 255);
        SDL_RenderDrawLine(renderer, 0, y, SDL_WIN_W, y);
    }
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
}

// Sprite procédural

void ViewGraph::renderCharacterSprite(Character* c, int cx, int cy, int size)
{
    if (!c) return;

    const string& name = c->getName();

    // // Hash couleur unique par personnage
    unsigned int hash = 5381;
    for (char ch : name) hash = hash * 33 + static_cast<unsigned char>(ch);

    // Teinte de base selon le type (utilisée par les deux chemins)
    int baseR, baseG, baseB;
    switch (c->typeC())
    {
        case Type_::atk_: baseR=160; baseG= 30; baseB= 30; break;
        case Type_::def_: baseR= 20; baseG= 60; baseB=180; break;
        case Type_::sup_: baseR= 20; baseG=140; baseB= 40; break;
        case Type_::int_: baseR=100; baseG= 20; baseB=160; break;
        case Type_::mag_: baseR=180; baseG=130; baseB=  0; break;
        case Type_::snp_: baseR=  0; baseG=140; baseB=160; break;
        default:          baseR= 60; baseG= 60; baseB= 60; break;
    }
    // Variation unique par perso (±32 sur chaque canal)
    int vR = (int)(hash & 0x3F) - 32;
    int vG = (int)((hash >> 6) & 0x3F) - 32;
    int vB = (int)((hash >> 12) & 0x3F) - 32;
    auto clamp = [](int v){ return (Uint8)(v < 0 ? 0 : v > 255 ? 255 : v); };
    SDL_Color bg = { clamp(baseR + vR), clamp(baseG + vG), clamp(baseB + vB), 230 };

    // // Bordure selon la rareté
    SDL_Color border;
    switch (c->rarC())
    {
        case Rarity::c:  border = {140, 140, 140, 255}; break;
        case Rarity::r:  border = { 60, 120, 255, 255}; break;
        case Rarity::sr: border = {160,  60, 255, 255}; break;
        case Rarity::e:  border = {255, 200,   0, 255}; break;
        case Rarity::l:  border = {255, 120,   0, 255}; break;
        case Rarity::m:  border = {255,  40,  40, 255}; break;
        default:         border = {180, 180, 180, 255}; break;
    }

    // // Portrait PNG par personnage (chargé à la demande depuis assets/)
    {
        auto it = charTex.find(name);
        if (it == charTex.end()) {
            // Premier accès : charger le PNG
            string path = "assets/char_" + name + ".png";
            SDL_Surface* s = IMG_Load(path.c_str());
            SDL_Texture* tex = nullptr;
            if (s) {
                tex = SDL_CreateTextureFromSurface(renderer, s);
                if (tex) SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND);
                SDL_FreeSurface(s);
            }
            charTex[name] = tex;
            it = charTex.find(name);
        }
        if (it->second) {
            int half = size / 2;
            SDL_Rect dst = {cx - half, cy - half, size, size};
            SDL_RenderCopy(renderer, it->second, nullptr, &dst);
            return;
        }
    }

    // // Fond (forme octogonale approchée)
    int half = size / 2;
    int cut  = size / 5;
    renderRect(cx - half + cut, cy - half,        size - cut * 2, size,        bg);
    renderRect(cx - half,       cy - half + cut,  size,           size - cut*2, bg);
    // Bordure
    renderOutlineRect(cx - half + cut, cy - half,        size - cut * 2, size,        border);
    renderOutlineRect(cx - half,       cy - half + cut,  size,           size - cut*2, border);
    // Double bordure pour épique+
    if (c->rarC() >= Rarity::e)
        renderOutlineRect(cx - half + cut + 1, cy - half + 1, size - cut*2 - 2, size - 2,
                          {255, 255, 180, 80});

    // // Initiales du personnage (jusqu'à 2 chars)
    // Construire les initiales : première lettre + première lettre après un espace
    string init;
    init += name[0];
    for (size_t i = 1; i < name.size() && init.size() < 2; i++)
        if (name[i-1] == ' ' || name[i-1] == '-') init += name[i];
    if (init.size() < 2 && name.size() >= 2) init += name[1];

    // Ombre
    renderRect(cx - half + cut + 1, cy - half + cut + 1, size - cut*2 - 2, size - cut*2 - 2,
               {0, 0, 0, 60});

    SDL_Color textCol = {255, 255, 255, 255};
    if (size >= 60)
    {
        // Grand sprite → initiales avec renderText (22px)
        int tx = cx - (int)(init.size() * 7);
        renderText(init, tx, cy - 13, textCol);
    }
    else if (size >= 30)
    {
        // Sprite moyen → renderTextSm (16px)
        int tx = cx - (int)(init.size() * 5);
        renderTextSm(init, tx, cy - 10, textCol);
    }
    // Sprite < 30px → juste une lettre via renderTextSm
    else if (size >= 18)
    {
        string first(1, name[0]);
        renderTextSm(first, cx - 4, cy - 9, textCol);
    }

    // // Indicateur de type (petit symbole coin bas-gauche)
    if (size >= 40)
    {
        int s = size / 6;
        int bx = cx - half + cut + 2, by = cy + half - cut - s - 2;
        switch (c->typeC())
        {
            case Type_::atk_: renderRect(bx, by, s, s*2, {255,200,100,220}); break; // épée
            case Type_::def_: renderRect(bx, by, s*2, s, {140,180,255,220}); break; // bouclier
            case Type_::sup_: renderRect(bx, by+s/2, s*2, s, {140,255,140,220});  // croix
                              renderRect(bx+s/2, by, s, s*2, {140,255,140,220}); break;
            case Type_::int_: renderRect(bx, by+s/2, s*2, s, {200,140,255,220}); break;
            case Type_::mag_: renderRect(bx, by, s*2, s*2, {255,220,80,180});
                              renderOutlineRect(bx, by, s*2, s*2, {255,240,120,255}); break;
            case Type_::snp_: renderRect(bx, by+s/2, s*2, s, {100,240,240,220});
                              renderRect(bx+s/2, by, s, s*2, {100,240,240,220}); break;
            default: break;
        }
    }

    // // Niveau (coin bas-droit si taille suffisante)
    if (size >= 40 && c->getLvl() > 1)
    {
        char buf[8];
        snprintf(buf, sizeof(buf), "%d", c->getLvl());
        int bx = cx + half - cut - 14, by = cy + half - 16;
        renderRect(bx, by, 14, 14, {0,0,0,180});
        renderTextSm(buf, bx + 1, by, {255,255,100,255});
    }
}

// Gestion souris

void ViewGraph::handleMouseClick(int x, int y)
{
    GameState state = game->getState();

    switch (state)
    {
        // Sélection sauvegarde
        // [N] Nouvelle Partie : x287..737 y195..245
        // Slot 1 : x162..862 y265..315 | Slot 2 : y330..380 | Slot 3 : y395..445
        // [Q] Quitter : x162..482 y460..504 | [R] Retour : x514..862 y460..504
        case GameState::SAVE_SELECT:
            if      (y >= 193 && y <= 247 && x >= 287 && x <= 737) fireInput('n');
            else if (y >= 263 && y <= 317 && x >= 162 && x <= 862) fireInput('1');
            else if (y >= 328 && y <= 382 && x >= 162 && x <= 862) fireInput('2');
            else if (y >= 393 && y <= 447 && x >= 162 && x <= 862) fireInput('3');
            else if (y >= 458 && y <= 506 && x >= 162 && x <= 482) fireInput('q');
            else if (y >= 458 && y <= 506 && x >= 514 && x <= 862) fireInput('r');
            break;

        // // Menu principal
        // renderButton("[J] Jouer",300,370,424,48) → y 370..418
        // renderButton("[P] Param", 300,430,424,48) → y 430..478
        // renderButton("[Q] Quitter",300,490,424,48) → y 490..538
        case GameState::MAIN_MENU:
            if      (y >= 370 && y <= 418 && x >= 300 && x <= 724) fireInput('j');
            else if (y >= 430 && y <= 478 && x >= 300 && x <= 724) fireInput('p');
            else if (y >= 490 && y <= 538 && x >= 300 && x <= 724) fireInput('q');
            break;

        // // Paramètres
        // Positions exactes tirées de renderSettings() :
        //  FPS bouton  : renderRect(550,108,130,28) → y 108..136
        //  Volume      : renderRect(450,158,90,28) / (560,158,90,28) → y 158..186
        //  Luminosité  : renderRect(450,213,90,28) / (560,213,90,28) → y 213..241
        //  Sauvegarder : renderRect(200,278,200,32) → y 278..310
        //  Retour      : renderRect(200,320,220,32) → y 320..352
        case GameState::SETTINGS:
            if (y >= 105 && y <= 140) fireInput('f');
            else if (y >= 155 && y <= 190)
            {
                if (x >= 558) fireInput('+');
                else          fireInput('-');
            }
            else if (y >= 210 && y <= 245)
            {
                if (x >= 558) fireInput('l');
                else          fireInput('k');
            }
            else if (y >= 275 && y <= 315) fireInput('s');
            else if (y >= 317 && y <= 358) fireInput('m');
            break;

        // // Menu jeu
        case GameState::PLAY_MENU:
            if      (y >= 198 && y <= 256) fireInput('b');
            else if (y >= 262 && y <= 320) fireInput('c');
            else if (y >= 326 && y <= 384) fireInput('s');
            else if (y >= 390 && y <= 448) fireInput('m');
            break;

        // // Bank : clic sur une ligne → ouvre le détail
        case GameState::BANK:
        {
            // Bouton Menu en pied de page : renderButton(800, 664, 180, 32)
            if (y >= 662 && y <= 698 && x >= 798 && x <= 982) { fireInput('m'); break; }
            if (y < 85 || y >= 660) break;
            int row = (y - 85) / 36;
            int idx = bankScrollStart + row;
            if (idx >= 0 && idx < game->getPlayer()->getNbBank())
            {
                game->setSelectedIndex(idx);
                capaScrollOffset = 0;
                Event ev(EventType::BANK_DETAIL);
                game->update(&ev);
            }
            break;
        }

        // // Détail personnage
        // Boutons rendus à y=720, h=36 dans renderBankDetail()
        case GameState::BANK_DETAIL:
        {
            Player*    p   = game->getPlayer();
            Character* c   = p->getBankCharacter(game->getSelectedIndex());
            bool       unlk = c ? p->isUnlocked(c->getName()) : false;
            if (y >= 718 && y <= 758)
            {
                if (!unlk && x >= 40 && x <= 300)  fireInput('d'); // Débloquer
                if ( unlk && x >= 40 && x <= 400)  fireInput('u'); // Améliorer
                if (x >= 597 && x <= 763)           fireInput('r'); // Retour
            }
            break;
        }

        // // Campagne : clic sur un arc
        case GameState::CAMPAIGN:
        {
            // Bouton Menu : renderButton(800, 710, 200, 34) → y 710..744
            if (y >= 708 && y <= 746 && x >= 798 && x <= 1002) { fireInput('m'); break; }
            if (y < 85 || y >= 700) break;
            int sel   = game->getCampaign()->getCurrentArcIdx();
            int start = (sel > 12) ? sel - 12 : 0;
            int row   = (y - 85) / 44;
            int idx   = start + row;
            if (idx >= 0 && idx < NB_ARCS)
            {
                game->setSelectedIndex(idx);
                Event ev(EventType::CAMPAIGN_ARC);
                game->update(&ev);
            }
            break;
        }

        // // Épisodes d'un arc
        case GameState::CAMPAIGN_ARC:
        {
            // Bouton Retour : renderButton(620, 710, 360, 34) → y 710..744
            if (y >= 708 && y <= 746 && x >= 618 && x <= 982) { fireInput('r'); break; }
            if (y < 130 || y >= 700) break;
            Arc* arc = game->getCampaign()->getCurrentArc();
            int  row = (y - 130) / 48;
            if (row >= 0 && row < arc->getNbEpisodes())
            {
                game->setSelectedIndex(row);
                Event ev(EventType::CAMPAIGN_EPISODE);
                game->update(&ev);
            }
            break;
        }

        // // Préparation combat
        // renderButton([C],40,600,210,36)  renderButton([E],270,600,190,36)
        // renderButton([R],480,600,130,36) renderButton([M],630,600,130,36)
        case GameState::BATTLE_PREPA:
            if (y >= 585 && y <= 628)
            {
                if (x >= 40  && x <= 250) fireInput('c');
                if (x >= 270 && x <= 460) fireInput('e');
                if (x >= 480 && x <= 610) fireInput('r');
                if (x >= 630 && x <= 760) fireInput('m');
            }
            break;

        // // Combat : clic = tour suivant
        case GameState::BATTLE:
        {
            Battle* b = game->getCurrentBattle();
            if (b && !b->isOver())
            {
                b->playNextTurn();
            }
            else if (b && b->isOver())
            {
                // Boutons fin de combat : y 524..558 (déplacés plus haut)
                // [J] Rejouer   : x 140..290
                // [C] Continuer : x 310..470
                // [R] Retour    : x 490..620
                // [M] Menu      : x 640..760
                if (y >= 522 && y <= 560)
                {
                    if (x >= 138 && x <= 292) fireInput('j'); // Rejouer
                    if (x >= 308 && x <= 472 && b->getWinner() == b->getPlayer()) fireInput('c'); // Continuer
                    if (x >= 488 && x <= 622) fireInput('r'); // Retour
                    if (x >= 638 && x <= 762) fireInput('m'); // Menu
                }
            }
            break;
        }

        // // Modification équipe
        case GameState::TEAM_CHANGE:
        {
            Player* p = game->getPlayer();
            // Clic sur la grille d'équipe : rangée 0 y=55..125, rangée 1 y=135..205
            if ((y >= 55 && y <= 125 || y >= 135 && y <= 205) && x >= 20 && x <= 20 + 5 * 190)
            {
                int col  = (x - 20) / 190;
                int row  = (y < 130) ? 0 : 1;
                int slot = row * 5 + col;
                if (slot >= 0 && slot < p->getTeamMax())
                {
                    Character* existing = (slot < p->getTeamSize()) ? p->getTeamCharacter(slot) : nullptr;
                    if (existing != nullptr)
                    {
                        // Clic sur un slot rempli → retirer le personnage
                        p->removeFromTeam(slot);
                        if (selectedTeamSlot >= p->getTeamSize())
                            selectedTeamSlot = -1;
                    }
                    else
                    {
                        // Clic sur un slot vide (ou nullptr) → sélectionner ce slot cible
                        selectedTeamSlot = slot;
                    }
                }
                break;
            }
            // Clic sur la liste des personnages disponibles (y 250..250+12*34)
            if (y >= 250 && y < 250 + 12 * 34)
            {
                int sel   = game->getSelectedIndex();
                int start = (sel > 10) ? sel - 10 : 0;
                int row   = (y - 250) / 34;
                int idx   = start + row;
                if (idx >= 0 && idx < p->getNbUnlock())
                {
                    game->setSelectedIndex(idx);
                    // Placer dans le slot sélectionné ou simplement ajouter
                    Character* c = p->getUnlockCharacter(idx);
                    if (selectedTeamSlot >= 0 && selectedTeamSlot < p->getTeamMax())
                    {
                        p->setTeamSlot(selectedTeamSlot, c);
                        selectedTeamSlot = -1;
                    }
                    else
                    {
                        game->confirmSelection();
                    }
                }
                break;
            }
            // Bouton Retour : renderRect(500,724,200,32) → y 724..756
            if (y >= 722 && y <= 758 && x >= 498 && x <= 702)
                fireInput('r');
            break;
        }

        // // Shop : clic sur un coffre
        // Coffres rendus à y = 100 + i*60, h=52 dans renderShop()
        case GameState::SHOP:
            if      (y >= 125 && y <= 177 && x >= 30 && x <= 990) fireInput('1');
            else if (y >= 185 && y <= 237 && x >= 30 && x <= 990) fireInput('2');
            else if (y >= 245 && y <= 297 && x >= 30 && x <= 990) fireInput('3');
            else if (y >= 305 && y <= 357 && x >= 30 && x <= 990) fireInput('4');
            else if (y >= 365 && y <= 417 && x >= 30 && x <= 990) fireInput('5');
            else if (y >= 700 && y <= 748 && x >= 548 && x <= 802) fireInput('m');
            break;

        default: break;
    }
}

// Rendu par état

void ViewGraph::renderSaveSelect()
{
    renderOnePieceBg();

    SDL_Color gold  = {255, 215,  0, 255};
    SDL_Color white = {255, 255, 255, 255};
    SDL_Color hint  = {190, 200, 240, 255};

    renderRect(100, 68, 824, 56, {0, 0, 0, 160});
    renderTextCentered("GRAND LINE LEGENDS", 76, gold);
    renderLine(100, 130, 924, 130, {80, 80, 80, 255});

    // Panneau sombre couvrant TOUT le contenu : titre → infos bas
    renderRect(100, 140, 824, 520, {0, 0, 0, 170});
    renderOutlineRect(100, 140, 824, 520, {70, 70, 120, 180});

    renderTextCentered("Choisissez votre partie", 158, white);

    // Bouton Nouvelle Partie (y=195)
    renderButton("[N]  Nouvelle Partie", 287, 195, 450, 50, {20, 60, 20, 210}, {40, 110, 40, 255});

    // Slots de sauvegarde (y=265, 330, 395 — h=52)
    const char* slotKeys[3] = {"[1]", "[2]", "[3]"};
    for (int i = 0; i < 3; i++)
    {
        int btnY = 265 + i * 65;
        string summary = SaveData::getSummary(SAVE_PATHS[i]);
        string label;
        SDL_Color base, hov;
        if (summary.empty())
        {
            label = string(slotKeys[i]) + "  Emplacement " + to_string(i+1) + " : Vide";
            base = {30, 30, 60, 190}; hov = {50, 50, 110, 255};
        }
        else
        {
            label = string(slotKeys[i]) + "  " + summary;
            base = {40, 55, 110, 200}; hov = {70, 95, 190, 255};
        }
        renderButton(label, 162, btnY, 700, 50, base, hov);
    }

    // Boutons Quitter / Retour (y=460, h=44 — dans le panneau)
    renderButton("[Q]  Quitter", 162, 460, 320, 44, {80, 20, 20, 210}, {140, 40, 40, 255});
    renderButton("[R]  Retour",  514, 460, 348, 44, {20, 40, 90, 210}, { 50, 80, 170, 255});

    // Infos lisibles en bas (y=515, 538 — dans le panneau)
    renderTextSm("Clic sur un emplacement vide = nouvelle partie   |   Clic sur une save = charger", 130, 516, hint);
    renderTextSm("[R] ou bouton Retour = revenir au menu principal", 130, 538, hint);
}

void ViewGraph::renderMainMenu()
{
    renderOnePieceBg();

    SDL_Color gold  = {255, 215,  0, 255};
    SDL_Color white = {255, 255, 255, 255};
    SDL_Color gray  = {200, 200, 220, 255};

    // Bandeau semi-transparent derrière le titre
    renderRect(100, 68, 824, 56, {0, 0, 0, 160});
    renderTextCentered("GRAND LINE LEGENDS", 76, gold);
    renderLine(100, 130, 924, 130, {80, 80, 80, 255});

    // Panneau sombre derrière les boutons pour garantir la lisibilité
    renderRect(240, 340, 544, 240, {0, 0, 0, 180});
    renderOutlineRect(240, 340, 544, 240, {80, 80, 140, 180});

    renderButton("[J]  Jouer",        300, 370, 424, 48, {30,50,110,220}, {60,90,180,255});
    renderButton("[P]  Parametres",   300, 430, 424, 48, {30,30,80,220},  {50,60,140,255});
    renderButton("[Q]  Quitter",      300, 490, 424, 48, {80,20,20,220},  {140,40,40,255});
    renderTextSm("[J] Jouer  =  choisir ou charger une sauvegarde", 270, 553, gray);
}

void ViewGraph::renderSettings()
{
    clear({10, 10, 30, 255});
    Settings* s = game->getSettings();
    char buf[80];

    SDL_Color gold  = {255, 215, 0, 255};
    SDL_Color white = {255, 255, 255, 255};
    SDL_Color btn   = {50, 80, 150, 200};

    renderText("PARAMETRES", 400, 30, gold);
    renderLine(100, 80, 924, 80, {80, 80, 80, 255});

    // FPS
    snprintf(buf, sizeof(buf), "FPS cible : %d", s->getFps());
    renderTextSm(buf, 200, 110, white);
    renderButton("[F] Basculer", 550, 108, 130, 28, btn, {80,120,220,255});

    // Volume
    snprintf(buf, sizeof(buf), "Volume : %d%%", s->getVolume());
    renderTextSm(buf, 200, 160, white);
    renderButton("[-] Moins", 450, 158, 90, 28, btn, {80,120,220,255});
    renderButton("[+] Plus",  560, 158, 90, 28, btn, {80,120,220,255});
    // Barre volume
    renderRect(200, 195, 400, 10, {60, 60, 80, 255});
    renderRect(200, 195, s->getVolume() * 4, 10, {80, 160, 255, 255});

    // Luminosité
    snprintf(buf, sizeof(buf), "Luminosite : %d%%", s->getBrightness());
    renderTextSm(buf, 200, 215, white);
    renderButton("[K] Moins", 450, 213, 90, 28, btn, {80,120,220,255});
    renderButton("[L] Plus",  560, 213, 90, 28, btn, {80,120,220,255});
    // Barre luminosité
    renderRect(200, 248, 400, 10, {60, 60, 80, 255});
    renderRect(200, 248, s->getBrightness() * 4, 10, {255, 220, 80, 255});

    renderButton("[S] Sauvegarder",  200, 278, 200, 32, btn, {80,120,220,255});
    renderButton("[M] Retour au menu", 200, 320, 220, 32, btn, {80,80,160,255});
}

void ViewGraph::renderPlayMenu()
{
    renderGradientBg({10, 10, 40, 255}, {25, 15, 60, 255});
    SDL_Color gold  = {255, 215, 0, 255};
    SDL_Color white = {255, 255, 255, 255};
    SDL_Color btn   = {40, 60, 120, 200};
    Player* p = game->getPlayer();
    char buf[80];

    renderTextCentered("GRAND LINE LEGENDS", 20, gold);

    snprintf(buf, sizeof(buf), "%s  |  Niv. %d  |  %d berries",
             p->getPseudo().c_str(), p->getAccountLvl(), p->getBerries());
    renderTextSm(buf, 170, 58, {200, 200, 200, 255});

    // Barre d'XP avec valeur courante / seuil
    {
        int xp  = p->getXP();
        int thr = p->getXPThreshold();
        snprintf(buf, sizeof(buf), "XP : %d / %d", xp, thr);
        renderTextSm(buf, 170, 77, {140, 200, 255, 255});
        // Barre de progression XP
        int barW = (thr > 0) ? (int)((float)xp / thr * 684) : 0;
        if (barW > 684) barW = 684;
        renderRect(170, 94, 684, 8, {30, 30, 80, 200});
        renderRect(170, 94, barW,  8, {80, 160, 255, 255});
    }

    renderLine(100, 108, 924, 108, {80, 80, 80, 255});

    renderButton("[B]  Bank",     300, 200, 424, 54, btn, {70,100,200,240});
    renderButton("[C]  Campagne", 300, 264, 424, 54, btn, {70,100,200,240});
    renderButton("[S]  Shop",     300, 328, 424, 54, btn, {70,100,200,240});
    renderButton("[M]  Menu",     300, 392, 424, 54, btn, {70,80,160,240});
}

void ViewGraph::renderBank()
{
    renderGradientBg({8, 16, 36, 255}, {15, 25, 55, 255});
    SDL_Color gold  = {255, 215, 0, 255};
    SDL_Color white = {255, 255, 255, 255};
    SDL_Color sel   = {255, 200, 50, 255};

    renderText("BANK DES PERSONNAGES", 280, 10, gold);
    SDL_Color hdr = {180,180,180,255};
    renderTextSm("Nom",    70,  55, hdr);
    renderTextSm("Type",   235, 55, hdr);
    renderTextSm("Rarete", 330, 55, hdr);
    renderTextSm("PV",     430, 55, hdr);
    renderTextSm("Niv",    495, 55, hdr);
    renderTextSm("Frags",  555, 55, hdr);
    renderTextSm("Unlock", 650, 55, hdr);
    renderLine(20, 78, 1004, 78, {80,80,80,255});

    Player* p      = game->getPlayer();
    int     selIdx = game->getSelectedIndex();
    // Scroll fluide : ajuster bankScrollStart seulement quand la sélection sort de la vue
    const int VISIBLE = 16;
    if (selIdx < bankScrollStart)
        bankScrollStart = selIdx;
    if (selIdx >= bankScrollStart + VISIBLE)
        bankScrollStart = selIdx - VISIBLE + 1;
    if (bankScrollStart < 0) bankScrollStart = 0;
    int  start = bankScrollStart;
    char buf[160];

    for (int i = start; i < p->getNbBank() && (i - start) < VISIBLE; i++)
    {
        Character* c    = p->getBankCharacter(i);
        SDL_Color  col  = (i == selIdx) ? sel : white;
        int        y    = 82 + (i - start) * 36;
        bool       unlk = p->isUnlocked(c->getName());
        int        frags = p->getFragments(c->getName());

        if (i == selIdx)
            renderRect(16, y - 2, 988, 32, {40, 60, 100, 200});

        // Sprite 28px
        renderCharacterSprite(c, 34, y + 14, 28);

        char nbuf[32];
        // Colonnes fixes pour alignement parfait
        renderTextSm(c->getName().substr(0, 14),                                  70,  y + 8, col);
        renderTextSm(Type_DisplayString(c->typeC()).substr(0, 9),                 235, y + 8, col);
        renderTextSm(Rarity_DisplayString(c->rarC()).substr(0, 8),               330, y + 8, col);
        snprintf(nbuf, sizeof(nbuf), "%d", c->getPVBase());
        renderTextSm(nbuf,                                                         430, y + 8, col);
        snprintf(nbuf, sizeof(nbuf), "Lv%d", c->getLvl());
        renderTextSm(nbuf,                                                         495, y + 8, col);
        if (frags > 0 || unlk) {
            snprintf(nbuf, sizeof(nbuf), "%d/100", frags);
        } else {
            snprintf(nbuf, sizeof(nbuf), "0/100");
        }
        renderTextSm(nbuf,                                                         555, y + 8, unlk ? SDL_Color{100,255,100,255} : col);
        renderTextSm(unlk ? "OUI" : "--",                                          650, y + 8,
                     unlk ? SDL_Color{100,255,100,255} : SDL_Color{200,100,100,255});
    }

    renderLine(20, 662, 1004, 662, {80,80,80,255});
    renderTextSm("[haut/bas] Naviguer    [Entree / Clic] Detail", 20, 672, {180,180,180,255});
    renderButton("[M]  Menu", 800, 664, 180, 32, {50,80,150,200}, {80,80,180,255});
}

void ViewGraph::renderBankDetail()
{
    renderGradientBg({8, 16, 40, 255}, {18, 28, 60, 255});
    SDL_Color gold  = {255, 215, 0, 255};
    SDL_Color white = {255, 255, 255, 255};
    SDL_Color btn   = {50, 80, 150, 200};

    Player*    p   = game->getPlayer();
    Character* c   = p->getBankCharacter(game->getSelectedIndex());
    if (!c) return;

    char buf[120];

    // Grand sprite à droite
    renderCharacterSprite(c, 880, 140, 140);

    renderText(c->getName(), 300, 10, gold);
    renderLine(20, 55, 1004, 55, {80,80,80,255});

    snprintf(buf, sizeof(buf), "Type   : %s", Type_DisplayString(c->typeC()).c_str());
    renderTextSm(buf, 50, 70, white);
    snprintf(buf, sizeof(buf), "Rarete : %s", Rarity_DisplayString(c->rarC()).c_str());
    renderTextSm(buf, 50, 100, white);
    snprintf(buf, sizeof(buf), "Niveau : %d", c->getLvl());
    renderTextSm(buf, 50, 130, white);
    snprintf(buf, sizeof(buf), "PV     : %d", c->getPVBase());
    renderTextSm(buf, 50, 160, white);
    snprintf(buf, sizeof(buf), "Vitesse: %d", c->getSpeed());
    renderTextSm(buf, 50, 190, white);
    snprintf(buf, sizeof(buf), "Haki des Rois : %d  |  Armement : %d  |  Observation : %d",
             c->getHakiR(), c->getHakiA(), c->getHakiO());
    renderTextSm(buf, 50, 220, white);

    renderLine(20, 256, 1004, 256, {80,80,80,255});

    // Capacités avec défilement (molette souris / touches)
    int nbCapa = c->getNbCapa();
    if (capaScrollOffset >= nbCapa) capaScrollOffset = (nbCapa > 0) ? nbCapa - 1 : 0;
    snprintf(buf, sizeof(buf), "Capacites (%d / %d) — molette pour defiler :", capaScrollOffset + 1, nbCapa);
    renderTextSm(buf, 50, 262, gold);

    int y = 292;
    for (int i = capaScrollOffset; i < nbCapa && (i - capaScrollOffset) < 9; i++, y += 44)
    {
        Capacity* cap = c->getTabCapa()[i];
        TypeC     t1  = cap->getType().t1;
        snprintf(buf, sizeof(buf), "%d. %-22s [%-10s] Dmg:%5d  Chance:%3d%%",
                 i + 1, cap->getName().c_str(), typeCToString(t1).c_str(),
                 cap->getDamage(), cap->getPercentage());
        renderTextSm(buf, 70, y, white);
    }

    // Indicateur scroll si il y a plus de capacités
    if (nbCapa > 9)
    {
        int sbH = 9 * 44;
        int sbY = 292 + (capaScrollOffset * sbH) / nbCapa;
        renderRect(1006, 292, 6, sbH, {40,40,80,255});
        renderRect(1006, sbY, 6, sbH / nbCapa + 4, {120,160,255,255});
    }

    renderLine(20, 688, 1004, 688, {80,80,80,255});

    int  frags = p->getFragments(c->getName());
    bool unlk  = p->isUnlocked(c->getName());
    int  cost  = c->upgradeCost();

    snprintf(buf, sizeof(buf), "Fragments : %d / %d  |  Debloque : %s",
             frags, UNLOCK_FRAGMENT_COST, unlk ? "OUI" : "NON");
    renderTextSm(buf, 50, 696, white);

    if (!unlk)
    {
        renderButton("[D] Debloquer (100 frags)", 40, 720, 260, 36,
                     {50,80,150,200}, {80,120,220,255});
    }
    else
    {
        bool canUpgrade = (frags >= UPGRADE_FRAGMENT_COST && p->getBerries() >= cost);
        snprintf(buf, sizeof(buf), "[U] Ameliorer (%d berries + 100 frags)", cost);
        SDL_Color uCol = canUpgrade ? SDL_Color{50,80,150,200} : SDL_Color{80,40,40,200};
        SDL_Color uHov = canUpgrade ? SDL_Color{80,120,220,255} : SDL_Color{120,60,60,255};
        renderButton(buf, 40, 720, 400, 36, uCol, uHov);
    }
    renderButton("[R] Retour", 600, 720, 160, 36, {50,80,150,200}, {80,80,160,255});
}

void ViewGraph::renderCampaign()
{
    renderGradientBg({5, 10, 30, 255}, {10, 20, 50, 255});
    SDL_Color gold  = {255, 215, 0, 255};
    SDL_Color white = {255, 255, 255, 255};
    SDL_Color gray  = {120, 120, 120, 255};

    renderText("CAMPAGNE", 420, 10, gold);
    renderTextSm("#   Nom de l'arc              Boss                  Statut", 20, 55, {180,180,180,255});
    renderLine(20, 75, 1004, 75, {80,80,80,255});

    Campaign* camp = game->getCampaign();
    int       sel  = camp->getCurrentArcIdx();
    int       start = (sel > 12) ? sel - 12 : 0;
    char      buf[120];

    for (int i = start; i < NB_ARCS && (i - start) < 14; i++)
    {
        Arc*      arc    = camp->getArc(i);
        int       y      = 80 + (i - start) * 44;
        bool      locked = (i > 0 && !camp->getArc(i - 1)->isCompleted());
        SDL_Color col    = locked ? gray : ((i == sel) ? gold : white);

        if (i == sel)
            renderRect(16, y - 2, 988, 40, {40,60,100,200});

        if (locked)
        {
            snprintf(buf, sizeof(buf), "%2d  %-26s %-22s [Verrouille]",
                     i + 1, arc->getName().c_str(), "---");
        }
        else
        {
            snprintf(buf, sizeof(buf), "%2d  %-26s %-22s %s",
                     i + 1, arc->getName().c_str(), arc->getBossName().c_str(),
                     arc->isCompleted() ? "[Termine]" : "");
        }
        renderTextSm(buf, 20, y, col);
    }

    renderLine(20, 700, 1004, 700, {80,80,80,255});
    renderTextSm("[haut/bas] Naviguer    [Entree/Clic] Choisir", 20, 714, {180,180,180,255});
    renderButton("[M]  Menu", 800, 710, 200, 34, {50,80,150,200}, {80,80,180,255});
}

void ViewGraph::renderCampaignArc()
{
    renderGradientBg({5, 10, 30, 255}, {10, 20, 50, 255});
    SDL_Color gold  = {255, 215, 0, 255};
    SDL_Color white = {255, 255, 255, 255};
    SDL_Color gray  = {120, 120, 120, 255};

    Campaign* camp = game->getCampaign();
    Arc*      arc  = camp->getCurrentArc();
    int       sel  = camp->getCurrentEpIdx();
    char      buf[120];

    snprintf(buf, sizeof(buf), "ARC : %s", arc->getName().c_str());
    renderText(buf, 320, 10, gold);

    snprintf(buf, sizeof(buf), "Boss : %s  |  Unlock : %s (%d%%)",
             arc->getBossName().c_str(),
             arc->getUnlockCharName().empty() ? "Aucun" : arc->getUnlockCharName().c_str(),
             static_cast<int>(arc->getUnlockChance() * 100));
    renderTextSm(buf, 50, 60, white);

    renderLine(20, 90, 1004, 90, {80,80,80,255});
    renderTextSm("#  Nom                         Type           Statut", 20, 95, {180,180,180,255});
    renderLine(20, 115, 1004, 115, {80,80,80,255});

    for (int i = 0; i < arc->getNbEpisodes(); i++)
    {
        Episode*  ep     = arc->getEpisode(i);
        int       y      = 120 + i * 48;
        bool      locked = (i > 0 && !arc->getEpisode(i - 1)->isCompleted());
        SDL_Color col    = locked ? gray : ((i == sel) ? gold : white);

        if (i == sel)
            renderRect(16, y - 2, 988, 44, {40,60,100,200});

        const char* type;
        if      (locked)         type = "[Verrouille]";
        else if (ep->getIsBoss())     type = "[BOSS]      ";
        else if (ep->getIsMiniBoss()) type = "[Mini-boss] ";
        else                     type = "[Combat]    ";

        snprintf(buf, sizeof(buf), "%d  %-28s %s  %s",
                 i + 1,
                 locked ? "???" : ep->getName().c_str(),
                 type,
                 (!locked && ep->isCompleted()) ? "(Termine)" : "");
        renderTextSm(buf, 20, y, col);
    }

    renderLine(20, 700, 1004, 700, {80,80,80,255});
    renderTextSm("[haut/bas] Naviguer    [Entree/Clic] Lancer", 20, 714, {180,180,180,255});
    renderButton("[R]  Retour aux arcs", 620, 710, 360, 34, {50,80,150,200}, {80,80,180,255});
}

void ViewGraph::renderBattlePrepa()
{
    clear({15, 20, 50, 255});
    SDL_Color gold  = {255, 215, 0, 255};
    SDL_Color white = {255, 255, 255, 255};
    SDL_Color btn   = {50, 80, 150, 200};

    Campaign* camp = game->getCampaign();
    Player*   p    = game->getPlayer();
    Arc*      arc  = camp->getCurrentArc();
    Episode*  ep   = camp->getCurrentEpisode();
    char      buf[120];

    snprintf(buf, sizeof(buf), "PREPARATION — %s : %s",
             arc->getName().c_str(), ep ? ep->getName().c_str() : "?");
    renderText(buf, 120, 10, gold);

    snprintf(buf, sizeof(buf), "Difficulte : %d  |  Equipe : %d / %d",
             ep ? ep->getDifficulty() : 0, p->getTeamSize(), p->getTeamMax());
    renderTextSm(buf, 50, 60, white);

    renderLine(20, 90, 1004, 90, {80,80,80,255});
    renderText("Votre equipe", 50, 100, gold);

    const int showMax = 5;
    for (int i = 0; i < p->getTeamSize() && i < showMax; i++)
    {
        Character* c = p->getTeamCharacter(i);
        if (!c) continue;
        snprintf(buf, sizeof(buf), "%d. %-16s  PV:%-5d  Vitesse:%d",
                 i + 1, c->getName().c_str(), c->getPV(), c->getSpeed());
        renderTextSm(buf, 70, 140 + i * 40, white);
    }
    if (p->getTeamSize() > showMax)
    {
        snprintf(buf, sizeof(buf), "  ... et %d autres", p->getTeamSize() - showMax);
        renderTextSm(buf, 70, 140 + showMax * 40, {150,150,150,255});
    }
    if (p->getTeamSize() == 0)
        renderTextSm("(Aucun personnage — allez dans Equipe)", 70, 140, {200,80,80,255});

    // // Apercu ennemis
    renderLine(20, 390, 1004, 390, {80,80,80,255});
    renderText("Ennemis prevus", 50, 396, {220, 60, 60, 255});
    if (ep)
    {
        const char* faction = "SoldatMarine";
        const string& arcName = arc->getName();
        if (arcName == "Orange Town" || arcName == "Village Sirop" || arcName == "Baratie")
            faction = "Pirate";
        else if (arcName == "Whiskey Peak" || arcName == "Alabasta" || arcName == "Little Garden")
            faction = "Agent Baroque";
        else if (arcName == "Thriller Bark")
            faction = "Zombie";

        if (ep->getIsBoss())
        {
            const char* bossName = (arcName == "Romance Dawn") ? "Morgan" : "Boss";
            snprintf(buf, sizeof(buf), "BOSS : %s  (PV ~%d)", bossName, 2000 + ep->getDifficulty() * 150);
            renderTextSm(buf, 70, 450, {255, 80, 80, 255});
            snprintf(buf, sizeof(buf), "+ 2 gardes (%s)", faction);
            renderTextSm(buf, 70, 480, {200, 100, 100, 255});
        }
        else if (ep->getIsMiniBoss())
        {
            const char* mbName = (arcName == "Romance Dawn") ? "Helmeppo" : "Mini-Boss";
            snprintf(buf, sizeof(buf), "MINI-BOSS : %s  (PV ~%d)", mbName, 1500 + ep->getDifficulty() * 200);
            renderTextSm(buf, 70, 450, {255, 140, 60, 255});
            snprintf(buf, sizeof(buf), "+ 1 garde (%s)", faction);
            renderTextSm(buf, 70, 480, {200, 140, 80, 255});
        }
        else
        {
            snprintf(buf, sizeof(buf), "3 ennemis normaux (%s)  PV ~%d chacun",
                     faction, 600 + ep->getDifficulty() * 80);
            renderTextSm(buf, 70, 450, {180, 180, 180, 255});
        }
        snprintf(buf, sizeof(buf), "Difficulte : %d", ep->getDifficulty());
        renderTextSm(buf, 70, 510, {160, 160, 100, 255});

        // Récompenses attendues
        int baseB = ep->getDifficulty() * 150;
        if (ep->getIsMiniBoss()) baseB = ep->getDifficulty() * 300;
        if (ep->getIsBoss())     baseB = ep->getDifficulty() * 500;
        int xpB = ep->getDifficulty() * 100;
        snprintf(buf, sizeof(buf), "Recompenses si victoire :  +%d berries   +%d XP", baseB, xpB);
        renderTextSm(buf, 70, 540, {100, 220, 100, 255});
        if (ep->getNbRewards() > 0)
        {
            int shown = 0;
            for (int ri = 0; ri < ep->getNbRewards() && shown < 2; ri++)
            {
                Reward* rw = ep->getReward(ri);
                if (!rw || rw->getFragmentCharName().empty()) continue;
                snprintf(buf, sizeof(buf), "  + %d frags %s  (%.0f%%)",
                         rw->getNbFragments(), rw->getFragmentCharName().c_str(),
                         rw->getDropChance() * 100.0f);
                renderTextSm(buf, 90, 552 + shown * 18, {80, 200, 80, 255});
                shown++;
            }
        }
    }

    renderLine(20, 582, 1004, 582, {80,80,80,255});

    SDL_Color btnH = {80, 120, 220, 255};
    renderButton("[C] Lancer le combat", 40,  588, 210, 36, btn, btnH);
    renderButton("[E] Gerer l'equipe",   270, 588, 190, 36, btn, btnH);
    renderButton("[R] Retour",           480, 588, 130, 36, btn, btnH);
    renderButton("[M] Menu",             630, 588, 130, 36, btn, btnH);
}

void ViewGraph::renderBattle()
{
    clear({5, 5, 20, 255});
    Battle* b = game->getCurrentBattle();
    if (b != lastBattle) { lastBattle = b; rewardApplied = false; battleRewardMsg = ""; }
    if (!b) return;

    SDL_Color gold  = {255, 215, 0, 255};
    SDL_Color white = {255, 255, 255, 255};
    SDL_Color red   = {220, 60,  60, 255};
    SDL_Color green = {60,  200, 80, 255};
    SDL_Color gray  = {120, 120, 120, 255};
    char      buf[120];

    snprintf(buf, sizeof(buf), "COMBAT — Tour %d", b->getTurn());
    renderText(buf, 390, 8, gold);

    // // Grille 2 lignes
    // Ligne 0 = joueur, Ligne 1 = ennemi. Cases numérotées 0..9.
    const int CELL_W = 96;
    const int CELL_H = 80;
    const int GRID_X = 12;
    const int ROW0_Y = 48;
    const int ROW1_Y = 148;

    Player* p  = b->getPlayer();
    Player* en = b->getEnemy();

    // En-têtes des lignes (fond + texte couleur forte)
    renderRect(GRID_X - 2, ROW0_Y - 20, 220, 18, {0, 60, 0, 200});
    renderTextSm("Ligne 0 — VOTRE EQUIPE", GRID_X, ROW0_Y - 20, {100, 255, 100, 255});
    renderRect(GRID_X - 2, ROW1_Y - 20, 180, 18, {60, 0, 0, 200});
    renderTextSm("Ligne 1 — ENNEMIS",      GRID_X, ROW1_Y - 20, {255, 100, 100, 255});

    // Cases joueur
    for (int i = 0; i < 10; i++)
    {
        int cx = GRID_X + i * CELL_W;
        SDL_Color cellBg = {20, 40, 20, 180};
        renderRect(cx, ROW0_Y, CELL_W - 4, CELL_H, cellBg);
        renderOutlineRect(cx, ROW0_Y, CELL_W - 4, CELL_H, {60, 120, 60, 255});

        snprintf(buf, sizeof(buf), "[%d]", i);
        renderTextSm(buf, cx + 4, ROW0_Y + 2, gray);

        if (i < p->getTeamSize() && p->getTeamCharacter(i))
        {
            Character* c   = p->getTeamCharacter(i);
            SDL_Color  col = (c->getPV() > 0) ? white : red;
            // Sprite 24px centré dans la cellule
            if (c->getPV() > 0)
                renderCharacterSprite(c, cx + (CELL_W - 4) / 2, ROW0_Y + 26, 24);
            string name = c->getName();
            if (name.size() > 7) name = name.substr(0, 7);
            renderTextSm(name, cx + 2, ROW0_Y + 42, col);
            // Barre de vie
            int maxPV = c->getPVBase() > 0 ? c->getPVBase() : 1;
            int barW  = ((c->getPV() > 0 ? c->getPV() : 0) * (CELL_W - 8)) / maxPV;
            renderRect(cx + 2, ROW0_Y + 60, CELL_W - 8, 8, {80, 0, 0, 255});
            renderRect(cx + 2, ROW0_Y + 60, barW,        8, green);
        }
    }

    // Cases ennemi
    for (int i = 0; i < 10; i++)
    {
        int cx = GRID_X + i * CELL_W;
        SDL_Color cellBg = {40, 15, 15, 180};
        renderRect(cx, ROW1_Y, CELL_W - 4, CELL_H, cellBg);
        renderOutlineRect(cx, ROW1_Y, CELL_W - 4, CELL_H, {120, 50, 50, 255});

        snprintf(buf, sizeof(buf), "[%d]", i);
        renderTextSm(buf, cx + 4, ROW1_Y + 2, gray);

        if (i < en->getTeamSize() && en->getTeamCharacter(i))
        {
            Character* c   = en->getTeamCharacter(i);
            SDL_Color  col = (c->getPV() > 0) ? white : red;
            if (c->getPV() > 0)
                renderCharacterSprite(c, cx + (CELL_W - 4) / 2, ROW1_Y + 26, 24);
            string name = c->getName();
            if (name.size() > 7) name = name.substr(0, 7);
            renderTextSm(name, cx + 2, ROW1_Y + 42, col);
            int maxPV = c->getPVBase() > 0 ? c->getPVBase() : 1;
            int barW  = ((c->getPV() > 0 ? c->getPV() : 0) * (CELL_W - 8)) / maxPV;
            renderRect(cx + 2, ROW1_Y + 60, CELL_W - 8, 8, {80, 0, 0, 255});
            renderRect(cx + 2, ROW1_Y + 60, barW,        8, red);
        }
    }

    // // Log de combat
    renderLine(20, 240, 1004, 240, {80,80,80,255});
    renderTextSm("Actions :", 20, 244, gold);
    int nb    = (b->getNbLog() > 8) ? 8 : b->getNbLog();
    int start = b->getNbLog() - nb;
    for (int i = start; i < b->getNbLog(); i++)
    {
        const BattleLog& l = b->getLog(i);
        snprintf(buf, sizeof(buf), "%s  -->  %s  [%s]  -%d",
                 l.attacker.c_str(), l.target.c_str(),
                 l.capa.c_str(), l.value);
        renderTextSm(buf, 40, 264 + (i - start) * 22, white);
    }

    // // Résultat / récompenses
    if (b->isOver())
    {
        if (!rewardApplied)
        {
            if (b->getWinner() == b->getPlayer())
            {
                game->applyRewards();
                // Capturer le message maintenant pour qu'il persiste tout l'écran de fin
                battleRewardMsg = game->getStatusMsg();
            }
            rewardApplied = true;
        }

        Player* winner = b->getWinner();
        bool    won    = (winner == b->getPlayer());
        bool    lvlUp  = (won && battleRewardMsg.find("NIVEAU") != string::npos);

        // Bannière principale (VICTOIRE / NIVEAU / DEFAITE)
        if (lvlUp)
        {
            // Extraire la partie "NIVEAU X !" du message
            size_t pos = battleRewardMsg.find("NIVEAU");
            string lvlPart = battleRewardMsg.substr(pos);
            size_t end = lvlPart.find("  ");
            if (end != string::npos) lvlPart = lvlPart.substr(0, end);
            // Grand cadre doré centré — ASCII uniquement, lisible avec toutes les polices
            renderRect(112, 420, 800, 54, {0, 0, 0, 230});
            renderRect(114, 422, 796, 50, {10, 100, 10, 240});
            renderOutlineRect(112, 420, 800, 54, {255, 210, 0, 255});
            renderOutlineRect(114, 422, 796, 50, {255, 235, 80, 200});
            renderTextCentered(">>> " + lvlPart + " ! <<<", 432, {255, 225, 0, 255});
        }
        else
        {
            SDL_Color bannerC = won ? SDL_Color{20,60,20,220} : SDL_Color{60,20,20,220};
            snprintf(buf, sizeof(buf), won ? "VICTOIRE !" : "DEFAITE...");
            renderRect(260, 420, 504, 46, bannerC);
            renderText(buf, won ? 390 : 400, 428, won ? gold : red);
        }

        // // Récompenses
        if (won && !battleRewardMsg.empty())
        {
            renderRect(100, 480, 824, 32, {10,40,10,200});
            renderTextSm(battleRewardMsg.c_str(), 110, 488, {100,255,120,255});
        }
        else if (!won)
        {
            renderRect(200, 480, 624, 32, {40,10,10,200});
            renderTextSm("Pas de recompense en cas de defaite.", 210, 488, {200,80,80,255});
        }

        // // Boutons (en dessous des récompenses, jamais recouverts)
        SDL_Color btnC = {50, 80, 150, 200};
        SDL_Color btnH = {80, 120, 220, 255};
        renderButton("[J] Rejouer",   140, 524, 150, 34, {80,40,40,200},  {140,60,60,255});
        if (won)
            renderButton("[C] Continuer", 310, 524, 160, 34, {40,80,40,200},  {60,140,60,255});
        renderButton("[R] Retour",    490, 524, 130, 34, btnC, btnH);
        renderButton("[M] Menu",      640, 524, 120, 34, btnC, btnH);
    }
    else
    {
        renderTextSm("Cliquez ou appuyez sur une touche pour le tour suivant...",
                     180, 742, {180,180,180,255});
    }
}

void ViewGraph::renderTeamChange()
{
    clear({10, 20, 50, 255});
    SDL_Color gold    = {255, 215,   0, 255};
    SDL_Color white   = {255, 255, 255, 255};
    SDL_Color gray    = {120, 120, 120, 255};
    SDL_Color green   = { 60, 200,  80, 255};
    SDL_Color selSlot = {255, 180,  30, 200};

    Player* p   = game->getPlayer();
    int     sel = game->getSelectedIndex();
    char    buf[120];

    renderText("MODIFIER L'EQUIPE", 350, 10, gold);

    // // Grille de positionnement (style Souls)
    // Rangée 0 : slots 0..4   Rangée 1 : slots 5..9
    // Chaque cellule : 190×70, début x=20, rangée 0 y=55, rangée 1 y=135
    renderTextSm("Grille d'equipe — clic slot vide pour cibler, slot rempli pour retirer :", 20, 38, gold);

    const int CW = 190, CH = 70;
    const int GX = 20,  GY0 = 55, GY1 = 135;

    for (int i = 0; i < 10; i++)
    {
        int col = i % 5;
        int row = i / 5;
        int cx  = GX + col * CW;
        int cy  = (row == 0) ? GY0 : GY1;

        bool locked = (i >= p->getTeamMax());
        bool filled = (!locked && i < p->getTeamSize() && p->getTeamCharacter(i) != nullptr);
        bool isSel  = (!locked && selectedTeamSlot == i);

        SDL_Color bg = locked ? SDL_Color{15,15,25,180}
                     : filled ? SDL_Color{20,60,20,200}
                              : SDL_Color{20,20,50,200};
        if (isSel) bg = selSlot;
        SDL_Color border = locked ? SDL_Color{40,40,60,200}
                         : isSel  ? SDL_Color{255,180,30,255}
                         : filled ? SDL_Color{60,180,60,255}
                                  : SDL_Color{60,60,120,255};
        renderRect(cx, cy, CW - 4, CH, bg);
        renderOutlineRect(cx, cy, CW - 4, CH, border);

        if (locked)
        {
            renderTextSm("VERROUILLE", cx + 30, cy + 28, {60,60,80,255});
        }
        else
        {
            snprintf(buf, sizeof(buf), "[%d]", i);
            renderTextSm(buf, cx + 4, cy + 4, gray);

            if (filled)
            {
                Character* c = p->getTeamCharacter(i);
                if (c)
                {
                    renderCharacterSprite(c, cx + CW/2 - 2, cy + 28, 32);
                    string name  = c->getName();
                    if (name.size() > 9) name = name.substr(0, 9);
                    snprintf(buf, sizeof(buf), "%s %d", name.c_str(), c->getPVBase());
                    renderTextSm(buf, cx + 4, cy + 50, white);
                }
            }
            else if (isSel)
            {
                renderTextSm("Choisir...", cx + 4, cy + 28, {255,200,60,255});
            }
        }
    }

    // // Liste des personnages disponibles
    renderLine(20, 220, 1004, 220, {80,80,80,255});
    if (selectedTeamSlot >= 0)
    {
        snprintf(buf, sizeof(buf), "Slot %d selectionne — cliquez un personnage pour l'y placer :",
                 selectedTeamSlot);
        renderTextSm(buf, 20, 228, {255,180,30,255});
    }
    else
    {
        renderTextSm("Personnages disponibles (clic pour ajouter / [<-] avec clavier) :", 20, 228, gold);
    }

    int start = (sel > 10) ? sel - 10 : 0;
    for (int i = start; i < p->getNbUnlock() && (i - start) < 12; i++)
    {
        Character* c   = p->getUnlockCharacter(i);
        int        y   = 250 + (i - start) * 34;
        SDL_Color  col = (i == sel) ? gold : white;
        if (i == sel) renderRect(16, y - 2, 988, 30, {40,60,100,200});
        renderCharacterSprite(c, 30, y + 14, 22);
        snprintf(buf, sizeof(buf), "%2d. %-14s  %-10s  PV:%5d  Vitesse:%3d",
                 i + 1,
                 c->getName().c_str(),
                 Type_DisplayString(c->typeC()).c_str(),
                 c->getPVBase(), c->getSpeed());
        renderTextSm(buf, 50, y + 8, col);
    }

    renderLine(20, 716, 1004, 716, {80,80,80,255});
    renderTextSm("[haut/bas] Naviguer  [<-] Ajouter  [Echap/R] Retour", 20, 720, {180,180,180,255});
    renderButton("[R] Retour", 500, 724, 200, 32, {50,80,150,200}, {80,80,160,255});
}

void ViewGraph::renderShop()
{
    renderGradientBg({10, 5, 25, 255}, {20, 10, 45, 255});
    SDL_Color gold  = {255, 215,   0, 255};
    SDL_Color white = {255, 255, 255, 255};
    SDL_Color btn   = { 50,  80, 150, 200};

    Shop*   shop = game->getShop();
    Player* p    = game->getPlayer();
    char    buf[140];

    renderText("SHOP", 430, 10, gold);

    snprintf(buf, sizeof(buf), "Vos berries : %d", p->getBerries());
    renderTextSm(buf, 50, 56, white);

    renderLine(20, 86, 1004, 86, {80,80,80,255});
    renderTextSm("Coffres disponibles — cliquez pour acheter :", 50, 92, gold);
    renderLine(20, 114, 1004, 114, {80,80,80,255});

    // Coffres : y = 125 + i*60, h=52
    static const SDL_Color COFFRE_COLORS[5] = {
        {30, 50,  80, 220}, {40, 30,  90, 220}, {50, 20,  80, 220},
        {70, 40,  20, 220}, {80, 10,  10, 220}
    };
    static const SDL_Color COFFRE_BORDER[5] = {
        {80, 140, 255, 255}, {160, 80, 255, 255}, {255, 180,  40, 255},
        {255, 120,  0, 255}, {255,  40,  40, 255}
    };
    static const char* COFFRE_DESC[5] = {
        "pool Commun/Rare — 25 a 60 frags (Rares: moins)",
        "pool Rare/SR — 18 a 40 frags (SR: moins)",
        "pool SR/Epique — 14 a 30 frags (Epique: moins)",
        "pool Epique/Legendaire — 12 a 25 frags (Leg: moins)",
        "pool Legendaire/Mythique — 10 a 22 frags (Myth: moins)"
    };
    for (int i = 0; i < NB_SHOP_PERMANENT; i++)
    {
        const ShopOffer& off = shop->getPermanent(i);
        int ry  = 125 + i * 60;
        bool hov = hovered(30, ry, 960, 52);

        SDL_Color bg  = hov ? SDL_Color{80,80,160,240} : COFFRE_COLORS[i];
        SDL_Color brd = hov ? SDL_Color{220,220,255,255} : COFFRE_BORDER[i];
        renderRect(30, ry, 960, 52, bg);
        renderOutlineRect(30, ry, 960, 52, brd);
        if (hov) renderOutlineRect(32, ry+2, 956, 48, {255,255,255,60});

        snprintf(buf, sizeof(buf), "[%d]  %s  —  %s  —  Prix : %d berries",
                 i + 1, off.label.c_str(), COFFRE_DESC[i], off.costBerries);
        renderTextSm(buf, 60, ry + 18, white);
    }

    renderLine(20, 435, 1004, 435, {80,80,80,255});
    renderTextSm("Coffre Rare       : C: 40-60 frags / R: 25-40 frags",          50, 445, {180,180,220,255});
    renderTextSm("Coffre Super Rare : R: 28-40 frags / SR: 18-30 frags",         50, 461, {160, 80,255,255});
    renderTextSm("Coffre Epique     : SR: 20-30 frags / E: 14-23 frags",         50, 477, {255,180, 40,255});
    renderTextSm("Coffre Legendaire : E: 16-25 frags / L: 12-20 frags",          50, 493, {255,120,  0,255});
    renderTextSm("Coffre Mythique   : L: 14-22 frags / M: 10-18 frags",          50, 509, {255, 80, 80,255});

    renderLine(20, 700, 1004, 700, {80,80,80,255});
    renderTextSm("[1][2][3][4][5] Acheter  ", 30, 710, {180,180,180,255});
    renderButton("[M]  Menu principal", 550, 710, 250, 36, btn, {80,80,180,255});
}

// Traitement des événements SDL

// Conversion fenêtre → coordonnées logiques robuste (Wayland/HiDPI/X11)
// Utilise la taille de sortie du renderer (pixels physiques) pour corriger
// Conversion coordonnées fenêtre → coordonnées logiques (SDL 2.0.18+).
// SDL_RenderWindowToLogical gère nativement HiDPI, Wayland, letter-boxing, etc.
static void winToLogical(SDL_Renderer* rend, int wx, int wy, int& lx, int& ly)
{
    float fx = 0.f, fy = 0.f;
    SDL_RenderWindowToLogical(rend, wx, wy, &fx, &fy);
    lx = (int)fx;
    ly = (int)fy;
}

void ViewGraph::handleSDLEvent(const SDL_Event& sdlEv)
{
    // Mouvement souris → hover
    if (sdlEv.type == SDL_MOUSEMOTION)
    {
        winToLogical(renderer, sdlEv.motion.x, sdlEv.motion.y, mouseX, mouseY);
        return;
    }

    // Clic souris
    if (sdlEv.type == SDL_MOUSEBUTTONDOWN && sdlEv.button.button == SDL_BUTTON_LEFT)
    {
        int lx, ly;
        winToLogical(renderer, sdlEv.button.x, sdlEv.button.y, lx, ly);
        handleMouseClick(lx, ly);
        return;
    }

    // Molette souris → scroll selon l'état
    if (sdlEv.type == SDL_MOUSEWHEEL)
    {
        int        scroll = sdlEv.wheel.y; // positif = molette vers le haut
        GameState  st     = game->getState();

        if (st == GameState::BANK_DETAIL)
        {
            Character* c = game->getPlayer()->getBankCharacter(game->getSelectedIndex());
            if (c)
            {
                if (scroll < 0 && capaScrollOffset < c->getNbCapa() - 1)
                    capaScrollOffset++;
                else if (scroll > 0 && capaScrollOffset > 0)
                    capaScrollOffset--;
            }
        }
        else if (st == GameState::BANK)
        {
            // Scroll updates both the view offset AND the selectedIndex
            int nb = game->getPlayer()->getNbBank();
            int sel = game->getSelectedIndex();
            sel -= scroll;
            if (sel < 0)      sel = 0;
            if (sel >= nb)    sel = (nb > 0) ? nb - 1 : 0;
            game->setSelectedIndex(sel);
            // bankScrollStart will auto-adjust in renderBank()
        }
        else if (st == GameState::TEAM_CHANGE)
        {
            int sel = game->getSelectedIndex();
            sel -= scroll;
            int maxSel = game->getPlayer()->getNbUnlock() - 1;
            if (sel < 0)      sel = 0;
            if (sel > maxSel) sel = maxSel;
            game->setSelectedIndex(sel);
        }
        else if (st == GameState::CAMPAIGN)
        {
            if (scroll > 0) game->getCampaign()->arcUp();
            else            game->getCampaign()->arcDown();
            game->setSelectedIndex(game->getCampaign()->getCurrentArcIdx());
        }
        else if (st == GameState::CAMPAIGN_ARC)
        {
            if (scroll > 0) game->getCampaign()->episodeUp();
            else            game->getCampaign()->episodeDown();
            game->setSelectedIndex(game->getCampaign()->getCurrentEpIdx());
        }
        return;
    }

    if (sdlEv.type != SDL_KEYDOWN) return;

    SDL_Keycode key = sdlEv.key.keysym.sym;
    char        c   = '\0';

    switch (key)
    {
        case SDLK_UP:     c = 65; break;
        case SDLK_DOWN:   c = 66; break;
        case SDLK_LEFT:   c = 68; break;
        case SDLK_RETURN: c = 10; break;
        case SDLK_PLUS:
        case SDLK_KP_PLUS:  c = '+'; break;
        case SDLK_MINUS:
        case SDLK_KP_MINUS: c = '-'; break;
        case SDLK_ESCAPE:
            // Echap en TEAM_CHANGE annule la sélection de slot
            if (game->getState() == GameState::TEAM_CHANGE)
                selectedTeamSlot = -1;
            return;
        default:
            if (key >= 32 && key < 128)
                c = static_cast<char>(key);
            break;
    }

    if (c == '\0') return;

    // BANK_DETAIL : flèches pour scroller les capacités
    if (game->getState() == GameState::BANK_DETAIL)
    {
        if (c == 65) { // haut
            if (capaScrollOffset > 0) capaScrollOffset--;
            return;
        }
        if (c == 66) { // bas
            Character* ch = game->getPlayer()->getBankCharacter(game->getSelectedIndex());
            if (ch && capaScrollOffset < ch->getNbCapa() - 1) capaScrollOffset++;
            return;
        }
    }

    // Réinitialiser scroll quand on quitte BANK_DETAIL
    if (c != 65 && c != 66 && game->getState() == GameState::BANK_DETAIL &&
        (c == 'r' || c == 'd' || c == 'u'))
        capaScrollOffset = 0;

    // BATTLE : touche quelconque = tour suivant
    if (game->getState() == GameState::BATTLE)
    {
        Battle* b = game->getCurrentBattle();
        if (b && !b->isOver())
        {
            b->playNextTurn();
            return;
        }
    }

    fireInput(c);
}

// Boucle SDL principale

void ViewGraph::run()
{
    if (!window || !renderer) return;

    game->init();

    // Ne rien pré-débloquer ici : Luffy est débloqué dans SAVE_NEW,
    // et les sauvegardes restaurent les personnages débloqués via SaveData::load.

    bool      running = true;
    SDL_Event ev;
    Settings* s = game->getSettings();

    while (running)
    {
        while (SDL_PollEvent(&ev))
        {
            if (ev.type == SDL_QUIT)
            {
                running = false;
                break;
            }
            handleSDLEvent(ev);
        }

        if (game->getState() == GameState::QUIT)
            running = false;

        // // Rendu selon l'état
        switch (game->getState())
        {
            case GameState::SAVE_SELECT:  renderSaveSelect();  break;
            case GameState::MAIN_MENU:    renderMainMenu();    break;
            case GameState::SETTINGS:     renderSettings();    break;
            case GameState::PLAY_MENU:    renderPlayMenu();    break;
            case GameState::BANK:         renderBank();        break;
            case GameState::BANK_DETAIL:  renderBankDetail();  break;
            case GameState::CAMPAIGN:     renderCampaign();    break;
            case GameState::CAMPAIGN_ARC: renderCampaignArc(); break;
            case GameState::BATTLE_PREPA: renderBattlePrepa(); break;
            case GameState::BATTLE:       renderBattle();      break;
            case GameState::TEAM_CHANGE:  renderTeamChange();  break;
            case GameState::SHOP:         renderShop();        break;
            default: break;
        }

        // Superpositions globales (ordre important)
        applyBrightnessOverlay();
        renderStatusMsg();

        present();

        int delay = (s->getFps() > 0) ? (1000 / s->getFps()) : 16;
        SDL_Delay(delay);
    }
}
