/**
 * @file Player.h
 * @brief Joueur avec sa progression, son équipe et sa bank de personnages.
 */

#ifndef PLAYER_H
#define PLAYER_H

#include <string>
#include "Characters.h"

/// @brief Coût en fragments pour débloquer un personnage
static const int UNLOCK_FRAGMENT_COST = 100;
/// @brief Coût en fragments pour améliorer un personnage d'un niveau
static const int UPGRADE_FRAGMENT_COST = 100;

/**
 * @struct FragEntry
 * @brief Association nom de personnage ↔ nombre de fragments possédés.
 */
struct FragEntry
{
    std::string charName;  ///< Nom du personnage
    int         count;     ///< Nombre de fragments
};

/**
 * @class Player
 * @brief Représente le joueur : compte, berries, bank, équipe et progression.
 *
 * La bank contient tous les personnages du jeu (débloqués ou non).
 * Les personnages débloqués peuvent être mis dans l'équipe.
 * L'équipe grossit en fonction du niveau de compte.
 */
class Player
{
    private:
        std::string pseudo;    ///< Pseudo du joueur

        int lvl;               ///< Niveau du joueur (legacy, remplacé par accountLvl)
        int accountLvl;        ///< Niveau du compte (débloque des slots d'équipe)
        int xp;                ///< XP accumulée
        int berries;           ///< Monnaie du jeu

        Character** bank;      ///< Tous les personnages disponibles dans le jeu
        int         nbBank;    ///< Nombre de personnages dans la bank
        int         bankMax;   ///< Capacité allouée du tableau bank

        Character** unlocked;  ///< Personnages débloqués (sous-ensemble de bank)
        int         nbUnlock;  ///< Nombre de personnages débloqués
        int         ulkMax;    ///< Capacité allouée du tableau unlocked

        Character** team;      ///< Personnages sélectionnés pour le prochain combat
        int         teamSize;  ///< Nombre de personnages dans l'équipe
        int         teamMax;   ///< Nombre de slots d'équipe disponibles (max 10)

        FragEntry*  fragments; ///< Tableau d'entrées fragments par personnage
        int         nbFrag;    ///< Nombre d'entrées
        int         maxFrag;   ///< Capacité allouée

    public:
        /// @brief Constructeur vide (joueur sans personnages, utilisé pour les ennemis IA)
        explicit Player(const std::string& p = "Player");
        /// @brief Constructeur depuis un fichier CSV de personnages
        Player(std::ifstream& file, const std::string& path, const std::string& p = "Player");
        /// @brief Destructeur
        ~Player();

        void changePseudo(const std::string& p);

        // Bank
        void        addToBank(Character* c);
        Character*  getBankCharacter(int index) const;
        int         getNbBank()  const;
        int         getBankMax() const;

        // Débloqués
        /// @brief Débloquer directement un personnage (sans coût)
        void       addToUnlocked(Character* c);
        /// @brief Débloquer un personnage via les fragments (coût = UNLOCK_FRAGMENT_COST)
        bool       unlockCharacter(Character* c);
        /// @brief Débloquer tous les personnages (mode triche / démo)
        void       unlockAll();
        Character* getUnlockCharacter(int index) const;
        int        getNbUnlock() const;
        int        getUlkMax()   const;
        /// @brief Vrai si le personnage est déjà dans unlocked
        bool       isUnlocked(const std::string& charName) const;

        // Équipe
        bool        addToTeam(Character* c);
        void        removeFromTeam(int index);
        /// @brief Place un personnage dans un slot précis (étend l'équipe si besoin)
        void        setTeamSlot(int slot, Character* c);
        void        addTeamSize(int nb);
        int         getTeamSize() const;
        int         getTeamMax()  const;
        Character*  getTeamCharacter(int index) const;
        void        randomTeam();
        void        clearTeam();

        // Fragments
        void             addFragments(const std::string& charName, int amount);
        int              getFragments(const std::string& charName) const;
        int              getNbFrag()         const;
        const FragEntry& getFragEntry(int i) const;

        // Progression
        std::string getPseudo()    const;
        int         getLvl()       const;
        int         getAccountLvl()const;
        int         getXP()        const;
        int         getBerries()   const;

        void setLvl(int l);
        void addLvl(int l);
        void setAccountLvl(int l);
        void setXP(int x);
        void setBerries(int b);
        void addBerries(int b);

        /// @brief Ajoute de l'XP et fait monter de niveau si le seuil est atteint
        void addXP(int amount);
        /// @brief Seuil d'XP pour passer au niveau suivant
        int  getXPThreshold() const;

        /// @brief Réinitialise les PV de toute l'équipe
        void resetTeamPV();
};

#endif
