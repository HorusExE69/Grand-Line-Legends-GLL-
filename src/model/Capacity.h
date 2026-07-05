/**
 * @file Capacity.h
 * @brief Capacité (attaque, soin ou effet) d'un personnage.
 */

#ifndef CAPACITY_H
#define CAPACITY_H

#include <string>
#include "Effects.h"
#include "Squares.h"
#include "Characters.h"
#include "Utils.h"

/**
 * @class Capacity
 * @brief Représente une capacité utilisable ou passive d'un personnage.
 *
 * Une capacité peut infliger des dégâts, soigner, ou appliquer un effet.
 * Les capacités passives (percentage >= 100) sont appliquées automatiquement.
 * Les capacités actives sont choisies aléatoirement selon leur pourcentage.
 */
class Capacity
{
    private:
        std::string nameCapa;   ///< Nom de la capacité
        int         damage;     ///< Valeur de dégâts de base
        int         heal;       ///< Valeur de soin de base
        int         percentage; ///< Poids de sélection (100 = passif)
        bool        isPassive;  ///< Vrai si la capacité est passive
        bool        activated;  ///< Vrai si la capacité a été activée ce tour

        Effect*  eft;     ///< Effet associé à la capacité (peut être nullptr)
        TupleTC  typeC;   ///< Types de la capacité (principal + secondaire)

        Square*  launcher;    ///< Case de l'attaquant (initialisée avant use())
        Square** tabTargets;  ///< Tableau des cases ciblées
        int      nbTargets;   ///< Nombre de cibles actuelles

    public:
        /// @brief Constructeur par défaut
        Capacity();
        /// @brief Constructeur depuis une ligne CSV du fichier de capacités
        /// @param line Ligne CSV : id,char,name,type1,type2,damage,heal,targetType,...
        Capacity(std::string line);
        /// @brief Destructeur
        ~Capacity();

        /// @brief Ajoute une case cible au tableau
        /// @param s Case à ajouter
        void addTarget(Square* s);

        /// @brief Applique la capacité sur toutes les cibles enregistrées
        void use();

        /// @brief Retourne le nom de la capacité
        const std::string& getName()       const;
        /// @brief Retourne la valeur de dégâts
        int                getDamage()     const;
        /// @brief Retourne la valeur de soin
        int                getHeal()       const;
        /// @brief Retourne le poids de sélection
        int                getPercentage() const;
        /// @brief Retourne les types de la capacité
        TupleTC            getType()       const;
        /// @brief Retourne vrai si la capacité est passive
        bool               getIsPassive()  const;
        /// @brief Retourne vrai si la capacité a été activée
        bool               getActivated()  const;
        /// @brief Retourne l'effet associé (peut être nullptr)
        Effect*            getEffect()     const;

        /// @brief Mutateur : définit la case de l'attaquant
        void setLauncher(Square* s);
};

#endif
