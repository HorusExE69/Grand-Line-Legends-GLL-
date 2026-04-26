/**
 * @file SaveData.h
 * @brief Système de sauvegarde / chargement de la progression du joueur.
 */

#ifndef SAVEDATA_H
#define SAVEDATA_H

#include <string>

class Player;
class Campaign;

/**
 * @class SaveData
 * @brief Gère la lecture et l'écriture de la progression dans un fichier texte.
 *
 * Le fichier de sauvegarde est un fichier texte structuré contenant :
 * pseudo, niveau de compte, XP, berries, personnages débloqués et fragments.
 */
class SaveData
{
    public:
        /// @brief Vérifie si un fichier de sauvegarde existe
        static bool exists(const std::string& path);

        /// @brief Retourne un résumé lisible de la sauvegarde ("Pseudo - Niv.X") ou "" si absente
        static std::string getSummary(const std::string& path);

        /// @brief Sauvegarde l'état du joueur dans un fichier
        static void save(const std::string& path, const Player& p, const Campaign& c);

        /// @brief Charge la progression dans le joueur (retourne faux si échec)
        static bool load(const std::string& path, Player& p, Campaign& c);
};

#endif
