/**
 * @file ViewGraph.h
 * @brief Vue graphique SDL2 du jeu.
 */

#ifndef VIEWGRAPH_H
#define VIEWGRAPH_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <map>
#include "../model/Utils.h"
#include "../model/Characters.h"

class Game;
class Battle;

/// @brief Largeur logique de la fenêtre SDL (résolution virtuelle fixe)
static const int SDL_WIN_W = 1024;
/// @brief Hauteur logique de la fenêtre SDL
static const int SDL_WIN_H = 768;

/**
 * @class ViewGraph
 * @brief Vue graphique SDL2 : gère la fenêtre, le renderer, la police et la boucle d'événements.
 *
 * Chaque état du jeu est rendu par une méthode renderXxx() dédiée.
 * La souris est prise en charge via handleMouseClick().
 * La fenêtre est à taille fixe (SDL_RenderSetLogicalSize gère le HiDPI et Wayland).
 */
class ViewGraph
{
    public:
        /// @brief Constructeur : initialise SDL, la fenêtre et le renderer
        explicit ViewGraph(Game* g);
        /// @brief Destructeur : libère toutes les ressources SDL
        ~ViewGraph();

        /// @brief Lance la boucle SDL principale
        void run();

    private:
        SDL_Window*   window;           ///< Fenêtre SDL
        SDL_Renderer* renderer;         ///< Renderer SDL
        TTF_Font*     font;             ///< Police principale
        TTF_Font*     fontSm;           ///< Police secondaire (taille réduite)
        Game*         game;             ///< Référence au contrôleur
        bool          rewardApplied;    ///< Récompenses déjà appliquées pour le combat courant
        Battle*       lastBattle;       ///< Dernier combat vu (pour détecter le changement)
        int           capaScrollOffset; ///< Décalage de défilement des capacités (BANK_DETAIL)
        int           bankScrollStart;  ///< Première ligne visible dans la bank
        int           mouseX;           ///< Position X souris (coords logiques)
        int           mouseY;           ///< Position Y souris (coords logiques)
        std::string   displayedMsg;     ///< Message de feedback en cours d'affichage
        Uint32        statusMsgExpiry;  ///< Timestamp SDL (ms) d'expiration du message
        int           selectedTeamSlot; ///< Slot de grille sélectionné en TEAM_CHANGE (-1=aucun)
        std::string   battleRewardMsg;  ///< Message de récompenses persisté pendant l'écran de fin de combat

        SDL_Texture*  bgTex;                            ///< Fond One Piece (assets/bg.png)
        std::map<std::string, SDL_Texture*> charTex;  ///< Portrait par nom de personnage (chargé à la demande)

        // Primitives de rendu
        void renderText  (const std::string& text, int x, int y,
                          SDL_Color col = {255,255,255,255});
        void renderTextSm(const std::string& text, int x, int y,
                          SDL_Color col = {200,200,200,255});
        void renderTextCentered(const std::string& text, int y,
                          SDL_Color col = {255,255,255,255});
        void renderRect        (int x, int y, int w, int h, SDL_Color col);
        void renderOutlineRect (int x, int y, int w, int h, SDL_Color col);
        void renderLine        (int x1, int y1, int x2, int y2, SDL_Color col);

        /// @brief Efface l'écran avec une couleur de fond
        void clear(SDL_Color bg);
        /// @brief Applique un overlay noir semi-transparent selon la luminosité
        void applyBrightnessOverlay();
        /// @brief Affiche le message de feedback (statusMsg) en superposition
        void renderStatusMsg();
        /// @brief Présente le renderer
        void present();

        // Rendu par état
        void renderSaveSelect();
        void renderMainMenu();
        void renderSettings();
        void renderPlayMenu();
        void renderBank();
        void renderBankDetail();
        void renderCampaign();
        void renderCampaignArc();
        void renderBattlePrepa();
        void renderBattle();
        void renderTeamChange();
        void renderShop();

        // Traitement des événements SDL
        void handleSDLEvent(const SDL_Event& ev);
        /// @brief Gère un clic souris (coordonnées logiques) selon l'état courant
        void handleMouseClick(int x, int y);
        /// @brief Déclenche un input clavier et met à jour le jeu
        void fireInput(char c);
        /// @brief Vrai si la souris est dans le rectangle (hover)
        bool hovered(int bx, int by, int bw, int bh) const;
        /// @brief Rendu d'un bouton avec effet hover automatique
        void renderButton(const std::string& label, int x, int y, int w, int h,
                          SDL_Color base, SDL_Color hover);
        /// @brief Sprite procédural d'un personnage (type + rareté → forme + couleur)
        void renderCharacterSprite(Character* c, int cx, int cy, int size);
        /// @brief Fond dégradé stylisé
        void renderGradientBg(SDL_Color top, SDL_Color bot);
        /// @brief Fond d'écran One Piece (ocean, lever de soleil, vagues, Jolly Roger)
        void renderOnePieceBg();
};

#endif
