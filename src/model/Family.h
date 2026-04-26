/**
 * @file Family.h
 * @brief Familles de personnages One Piece donnant des bonus d'équipe.
 */

#ifndef FAMILY_H
#define FAMILY_H

#include <string>

/// @brief Nombre maximum de membres dans une famille
static const int FAMILY_MAX_MEMBERS = 15;
/// @brief Nombre de familles définies dans le jeu
static const int NB_FAMILIES = 8;

/**
 * @class Family
 * @brief Représente une famille de personnages One Piece.
 *
 * Lorsque plusieurs membres d'une même famille sont dans l'équipe,
 * un bonus est appliqué (% attaque, % PV ou % défense) selon le nombre de membres présents.
 */
class Family
{
    private:
        std::string name;                          ///< Nom de la famille
        std::string members[FAMILY_MAX_MEMBERS];   ///< Noms des membres
        int         nbMembers;                     ///< Nombre de membres définis

        // Bonus selon le nombre de membres actifs dans l'équipe
        float       bonusAtk;  ///< Bonus d'attaque (ex : 0.20 = +20%)
        float       bonusPV;   ///< Bonus de PV
        float       bonusDef;  ///< Bonus de défense

    public:
        /// @brief Constructeur
        Family(const std::string& familyName, float atk, float pv, float def);

        /// @brief Ajoute un membre à la famille
        void addMember(const std::string& charName);

        /// @brief Retourne vrai si le personnage appartient à cette famille
        bool hasMember(const std::string& charName) const;

        /// @brief Compte combien de membres d'une liste de noms appartiennent à la famille
        int countPresent(const std::string* teamNames, int teamSize) const;

        std::string getName()     const;
        float       getBonusAtk() const;
        float       getBonusPV()  const;
        float       getBonusDef() const;
        int         getNbMembers()const;
};

/**
 * @brief Initialise et retourne le tableau statique des NB_FAMILIES familles définies.
 *
 * Le tableau est alloué sur le tas ; l'appelant doit le libérer.
 * @return Tableau de Family* de taille NB_FAMILIES
 */
Family** createFamilies();

#endif
