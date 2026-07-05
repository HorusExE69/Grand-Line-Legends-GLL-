/**
 * @file Characters.h
 * @brief Personnage jouable ou ennemi du jeu.
 */

#ifndef PERSONNAGE_H
#define PERSONNAGE_H

#include <string>
#include "Capacity.h"
#include "Squares.h"
#include "Utils.h"

class Capacity;

/**
 * @class Character
 * @brief Représente un personnage One Piece avec ses stats, capacités et état de combat.
 *
 * Les stats de base sont chargées depuis Characters.csv.
 * Les capacités sont chargées depuis data/capacities/<nom>.csv.
 * Le niveau peut être amélioré via la bank (coût en berries).
 */
class Character
{
    private:
        std::string name;     ///< Nom du personnage

        Type_  type;          ///< Rôle en combat
        Rarity rarity;        ///< Rareté

        Square* pos;          ///< Case occupée sur la grille de combat

        int pv;               ///< Points de vie actuels
        int pvBase;           ///< Points de vie de base (niveau 1)
        int hakiR;            ///< Haki des Rois (boost/debuff selon différence avec la cible)
        int hakiA;            ///< Haki de l'Armement (bonus de dégâts)
        int hakiO;            ///< Haki de l'Observation (chance d'esquive)
        int lvl;              ///< Niveau actuel du personnage

        Capacity** tabCapa;   ///< Tableau de capacités
        int        nbCapa;    ///< Nombre de capacités chargées
        int        maxCapa;   ///< Capacité du tableau

        Effect** tabEffects;  ///< Effets actifs en combat
        int      nbEffects;   ///< Nombre d'effets actifs
        int      maxEffects;  ///< Capacité du tableau d'effets

        int  speed;           ///< Vitesse (détermine l'ordre d'attaque)
        int  resistValue;     ///< Valeur de résistance aux dégâts
        int  bleedingValue;   ///< Valeur de saignement actif
        bool isBleeding;      ///< Vrai si le personnage saigne
        bool stunned;         ///< Vrai si le personnage est étourdi ce tour

    public:
        /// @brief Constructeur par défaut
        Character(void);
        /// @brief Constructeur depuis une ligne CSV et le chemin des capacités
        Character(const std::string& line, const std::string& pathCapaFolder);
        /// @brief Destructeur
        ~Character(void);

        /// @brief Charge les capacités depuis le fichier CSV dédié au personnage
        void loadCapacities(const std::string& path);

        // Tests de type
        Type_ typeC(void);
        bool  isSup(void);  ///< Vrai si type == sup
        bool  isDef(void);  ///< Vrai si type == def
        bool  isInt(void);  ///< Vrai si type == int
        bool  isAtk(void);  ///< Vrai si type == atk
        bool  isSnp(void);  ///< Vrai si type == snp
        bool  isMag(void);  ///< Vrai si type == mag

        // Tests de rareté
        Rarity rarC(void);
        bool   isC(void);   ///< Commun
        bool   isR(void);   ///< Rare
        bool   isSR(void);  ///< Super Rare
        bool   isE(void);   ///< Épique
        bool   isL(void);   ///< Légendaire
        bool   isM(void);   ///< Mythique

        /// @brief Augmente le niveau et met à jour les stats
        void updateLvl(int nbLvl);

        std::string getName(void);
        int         getPV(void);
        int         getPVBase(void)      const;
        int         getLvl(void)         const;
        int         getSpeed(void)       const;
        int         getHakiR(void)       const;
        int         getHakiO(void)       const;
        int         getHakiA(void)       const;
        int         getResistValue()     const;
        int         getBleedingValue()   const;
        bool        getIsBleeding()      const;
        bool        isStunned()          const;

        /// @brief Mutateurs état de combat
        void setStunned(bool val);

        /// @brief Applique un effet de combat au personnage
        void applyEffect(Effect* e);
        /// @brief Choisit aléatoirement une capacité active (selon les pourcentages)
        Capacity* chooseCapa();
        /// @brief Applique toutes les capacités passives du personnage
        void applyPassives();

        /// @brief Ajoute une capacité au tableau (redimensionnement si nécessaire)
        void addToCapa(Capacity* c);

        int         getNbCapa()   const;
        Capacity**  getTabCapa()  const;

        /// @brief Remet le personnage à ses PV max (entre les combats)
        void resetPV();
        /// @brief Réinitialise l'état de combat (effets, saignement, étourdissement)
        void resetCombatState();

        /// @brief Coût en berries pour monter du niveau courant au suivant
        int upgradeCost() const;
};

#endif
