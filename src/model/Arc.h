/**
 * @file Arc.h
 * @brief Arc de la campagne One Piece avec ses épisodes et récompenses.
 */

#ifndef ARC_H
#define ARC_H

#include "Episode.h"
#include <string>

/// @brief Nombre maximum de personnages bannis dans un arc
static const int MAX_BANNED = 10;

/**
 * @class Arc
 * @brief Représente un arc narratif (ex : Romance Dawn, Alabasta…).
 *
 * Chaque arc contient une liste d'épisodes croissants en difficulté,
 * se terminant par un mini-boss puis le boss principal.
 */
class Arc
{
    public:
        std::string name;           ///< Nom de l'arc
        std::string bossName;       ///< Nom du boss final

        std::string unlockCharName; ///< Personnage débloqué à la fin de l'arc
        float       unlockChance;   ///< Probabilité de débloquer le personnage (1.0 = 100%)

        std::string bannedChars[MAX_BANNED]; ///< Personnages non disponibles dans cet arc
        int         nbBanned;                ///< Nombre de personnages bannis

        Episode**   episodes;    ///< Tableau d'épisodes
        int         nbEpisodes;  ///< Nombre d'épisodes
        int         maxEpisodes; ///< Capacité du tableau

        bool        completed;   ///< Vrai si le boss a été vaincu

        /// @brief Constructeur
        Arc(const std::string& arcName, const std::string& boss,
            const std::string& unlock, float chance);
        /// @brief Destructeur
        ~Arc();

        /// @brief Ajoute un épisode à l'arc
        void addEpisode(Episode* ep);
        /// @brief Ajoute un personnage banni pour cet arc
        void addBanned(const std::string& charName);
        /// @brief Vérifie si un personnage est banni dans cet arc
        bool isBanned(const std::string& charName) const;
};

#endif
