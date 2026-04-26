/**
 * @file Reward.h
 * @brief Récompense obtenue à la fin d'un épisode ou d'un arc.
 */

#ifndef REWARD_H
#define REWARD_H

#include <string>

/**
 * @class Reward
 * @brief Représente une récompense : berries, fragments de personnage ou coffre.
 */
class Reward
{
    private:
        int          berries;           ///< Nombre de berries gagnées
        std::string  fragmentCharName;  ///< Nom du personnage dont on gagne des fragments
        int          nbFragments;       ///< Nombre de fragments gagnés
        float        dropChance;        ///< Probabilité d'obtenir cette récompense (1.0 = 100%)

    public:
        /// @brief Constructeur
        Reward(int berries=0, const std::string& charName="", int frags=0, float chance=1.0f);

        /// @brief Accesseurs
        int         getBerries()          const;
        std::string getFragmentCharName() const;
        int         getNbFragments()      const;
        float       getDropChance()       const;
};

#endif
