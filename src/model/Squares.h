/**
 * @file Squares.h
 * @brief Case de la grille de combat.
 */

#ifndef SQUARE_H
#define SQUARE_H

class Character;

/**
 * @class Square
 * @brief Représente une case de la grille BattleMap (2×10).
 *
 * Chaque case connaît sa position (x, y), si elle est occupée et quel
 * personnage (Character*) elle contient.
 */
class Square
{
    public:
        int  x;      ///< Ligne (0 = joueur, 1 = ennemi)
        int  y;      ///< Colonne (0-9)
        bool fill;   ///< Vrai si la case est occupée par un personnage

        Character* inmate; ///< Personnage occupant la case (nullptr si vide)

        /**
         * @brief Constructeur
         * @param posX  Ligne de la case
         * @param posY  Colonne de la case
         * @param C     Personnage initial (nullptr par défaut)
         */
        Square(int posX = 0, int posY = 0, Character* C = nullptr);
        /// @brief Destructeur
        ~Square(void);

        /// @brief Place un personnage sur la case
        /// @param C Personnage à placer
        void hold(Character* C);

        /// @brief Retire le personnage de la case
        void unhold(void);

        /// @brief Échange les occupants de deux cases
        /// @param S Case cible
        void swap(Square* S);

        /// @brief Retourne vrai si la case est vide
        bool isEmpty(void);
};

#endif
