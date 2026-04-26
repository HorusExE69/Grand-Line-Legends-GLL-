/**
 * @file Battle.h
 * @brief Contrôleur de combat entre deux joueurs sur la grille.
 */

#ifndef BATTLE_H
#define BATTLE_H

#include "../model/BattleMap.h"
#include "../model/Player.h"
#include "../model/Characters.h"

/// @brief Log d'un événement de combat (attaque, soin, effet)
struct BattleLog
{
    std::string attacker; ///< Nom de l'attaquant
    std::string capa;     ///< Nom de la capacité utilisée
    std::string target;   ///< Nom de la cible
    int         value;    ///< Valeur de dégâts / soin
};

/// @brief Taille maximale du log de combat
static const int MAX_LOG = 64;

/**
 * @class Battle
 * @brief Gère le déroulement d'un combat tour par tour entre le joueur et l'ennemi.
 *
 * Le combat se joue sur une grille BattleMap (2×10).
 * L'ordre d'attaque est déterminé par la vitesse des personnages.
 */
class Battle
{
    private:
        BattleMap* map;     ///< Grille de combat
        Player*    player;  ///< Joueur humain (non possédé)
        Player*    enemy;   ///< Équipe ennemie (possédée et libérée par Battle)
        int        turn;    ///< Numéro du tour courant

        BattleLog  log[MAX_LOG]; ///< Historique des actions du tour courant
        int        nbLog;        ///< Nombre d'entrées dans le log

    public:
        /// @brief Constructeur : crée la grille et prépare le joueur IA
        explicit Battle(Player* p = nullptr);
        ~Battle(void);

        /// @brief Initialise la grille avec les équipes
        void init();

        /// @brief Joue tous les tours jusqu'à la fin du combat
        void playAllTurns();
        /// @brief Joue le tour courant et incrémente le compteur
        void playTurn();
        /// @brief Joue un seul tour (retourne faux si le combat est terminé)
        bool playNextTurn();

        // État du combat
        bool    isDead(Player* p) const;
        bool    isOver()          const;
        Player* getWinner()       const;

        // Accesseurs
        Player*   getPlayer() const;
        Player*   getEnemy()  const;
        int       getTurn()   const;

        /// @brief Retourne l'ordre d'attaque trié par vitesse (tableau alloué sur le tas)
        Square** getOrder(int& outSize) const;

        // Log
        int              getNbLog()          const;
        const BattleLog& getLog(int index)   const;
        void             clearLog();

    private:
        /// @brief Fait jouer un personnage sur une case donnée
        void playCharacter(Square* sq, bool playerSide);
        /// @brief Ajoute une entrée dans le log
        void addLog(const std::string& attacker, const std::string& capa,
                    const std::string& target, int value);
};

#endif
