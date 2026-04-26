/**
 * @file Episode.h
 * @brief Épisode d'un arc de la campagne.
 */

#ifndef EPISODE_H
#define EPISODE_H

#include <string>
#include "Reward.h"

/**
 * @class Episode
 * @brief Représente un épisode (combat) au sein d'un arc de la campagne.
 *
 * Les premiers épisodes affrontent des sbires, l'avant-dernier un mini-boss,
 * et le dernier le boss principal de l'arc.
 */
class Episode
{
    public:
        std::string name;       ///< Nom de l'épisode
        int         difficulty; ///< Niveau de difficulté (nombre d'ennemis / leurs stats)
        bool        isMiniBoss; ///< Vrai si l'épisode est un mini-boss
        bool        isBoss;     ///< Vrai si l'épisode est le boss final
        bool        completed;  ///< Vrai si l'épisode a déjà été complété

        Reward**    rewards;    ///< Tableau de récompenses possibles
        int         nbRewards;  ///< Nombre de récompenses dans le tableau

        /// @brief Constructeur
        Episode(const std::string& n, int diff, bool miniBoss, bool boss);
        /// @brief Destructeur
        ~Episode();
        /// @brief Ajoute une récompense possible à l'épisode
        void addReward(Reward* r);
};

#endif
