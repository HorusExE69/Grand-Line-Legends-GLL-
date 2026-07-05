/**
 * @file BattleMap.h
 * @brief Grille de combat 2x10 utilisée pendant les batailles.
 */

#ifndef BATTLEMAP_H
#define BATTLEMAP_H

#include "Squares.h"
#include "Player.h"

/**
 * @class BattleMap
 * @brief Représente la grille de combat composée de deux lignes de 10 cases.
 *
 * La ligne 0 est occupée par le joueur, la ligne 1 par l'ennemi.
 * Chaque case (Square) peut contenir un personnage (Character*).
 */
class BattleMap
{
    private:
        int sizeMap; ///< Nombre total de cases par ligne (toujours 10)
        int nbFree;  ///< Nombre de cases libres (non utilisé en cours de combat)

        Square tabBox[2][10]; ///< Grille [ligne][colonne] : ligne 0 = joueur, ligne 1 = ennemi

    public:
        /// @brief Constructeur : initialise la grille vide
        BattleMap(void);
        /// @brief Destructeur
        ~BattleMap(void);

        /// @brief Place les personnages des deux joueurs sur la grille
        /// @param p Joueur humain (ligne 0)
        /// @param e Joueur ennemi (ligne 1)
        void init(Player* p, Player* e);

        /// @brief Retourne le nombre de cases par ligne
        int getSize(void);
        /// @brief Retourne le nombre de cases libres
        int getNbFree(void);
        /// @brief Retourne la case à la position (x, y)
        /// @param x Ligne (0 = joueur, 1 = ennemi)
        /// @param y Colonne (0-9)
        Square& getSquare(int x, int y);
};

#endif
