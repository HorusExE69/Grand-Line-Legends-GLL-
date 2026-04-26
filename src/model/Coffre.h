/**
 * @file Coffre.h
 * @brief Coffre (loot box) que le joueur peut ouvrir pour gagner des récompenses.
 */

#ifndef COFFRE_H
#define COFFRE_H

#include "Utils.h"
#include "Reward.h"

/**
 * @class Coffre
 * @brief Représente un coffre dont l'ouverture offre une récompense aléatoire.
 *
 * La rareté du coffre détermine la probabilité d'obtenir des personnages rares.
 */
class Coffre
{
    private:
        Rarity   rarity;     ///< Rareté du coffre (détermine les récompenses)
        bool     opened;     ///< Vrai si le coffre a déjà été ouvert

    public:
        /// @brief Constructeur
        explicit Coffre(Rarity r = Rarity::c);
        ~Coffre();

        /// @brief Ouvre le coffre et retourne une récompense (alloué sur le tas)
        Reward* open();

        bool   isOpened() const;
        Rarity getRarity() const;
};

#endif
