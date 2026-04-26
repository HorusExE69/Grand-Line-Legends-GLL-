/**
 * @file ViewText.h
 * @brief Vue textuelle du jeu utilisant la librairie WinTXT.
 */

#ifndef VIEWTEXT_H
#define VIEWTEXT_H

#include <string>
#include "winTxt.h"
#include "../model/Player.h"
#include "../model/Utils.h"

class Game;

/// @brief Largeur de la fenêtre texte (colonnes)
static const int WIN_W = 80;
/// @brief Hauteur de la fenêtre texte (lignes)
static const int WIN_H = 24;

/**
 * @class ViewText
 * @brief Vue entièrement textuelle s'appuyant sur WinTXT.
 *
 * Utilise win.print(x, y, char*) et win.draw() pour l'affichage.
 * Utilise win.getCh() pour la saisie clavier (non bloquante).
 * Pas de cout/cin.
 */
class ViewText
{
    private:
        Game*  game;              ///< Référence au contrôleur
        bool   running;           ///< Boucle principale active
        bool   rewardApplied;     ///< Vrai si les récompenses du combat courant ont été appliquées
        int    capaScrollOffset;  ///< Décalage de défilement pour les capacités (BANK_DETAIL)
        WinTXT win;               ///< Fenêtre textuelle 80×24

        // Utilitaires d'affichage
        /// @brief Efface le buffer WinTXT
        void clearBuf();
        /// @brief Imprime une chaîne à (x, y) dans le buffer
        void printAt(int x, int y, const std::string& s);
        /// @brief Imprime une ligne horizontale de tirets à la ligne y
        void hline(int y);
        /// @brief Dessine le buffer dans le terminal
        void draw();

        // Lecture d'une chaîne au clavier
        /// @brief Lit une chaîne caractère par caractère et l'affiche à (x,y)
        std::string readString(int x, int y, int maxLen = 16);

        // Affichage par état
        void renderMainMenu();
        void renderSettings();
        void renderPlayMenu();
        void renderBank();
        void renderBankDetail();
        void renderCampaign();
        void renderCampaignArc();
        void renderBattlePrepa();
        void renderBattle();
        void renderTeam();
        void renderTeamChange();
        void renderShop();
        void renderQuit();

    public:
        /// @brief Constructeur par défaut (sans jeu lié)
        ViewText();
        /// @brief Constructeur avec référence au contrôleur
        explicit ViewText(Game* g);
        ~ViewText(void);

        /// @brief Affiche l'écran correspondant à l'état courant du jeu
        void display();
        /// @brief Lance la boucle principale (lit les touches, envoie des Event au jeu)
        void run();
        /// @brief Retourne vrai tant que la boucle tourne
        bool isRunning() const;
};

#endif
