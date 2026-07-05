/**
 * @file Game.h
 * @brief Contrôleur principal du jeu — orchestre la logique et les transitions d'état.
 */

#ifndef GAME_H
#define GAME_H

#include "../model/Player.h"
#include "../model/Campaign.h"
#include "../model/Settings.h"
#include "../model/Shop.h"
#include "../model/SaveData.h"
#include "Event.h"
#include "Battle.h"

/// @brief Chemins des emplacements de sauvegarde (slots 1, 2, 3)
static const char* SAVE_PATHS[3] = {
    "./data/save1.txt",
    "./data/save2.txt",
    "./data/save3.txt"
};

/**
 * @class Game
 * @brief Contrôleur central : gère les transitions d'état, les entrées et la logique de jeu.
 *
 * Game est le pivot MVC : il reçoit les Event émis par la vue et met à jour le modèle
 * en conséquence. Il ne connaît pas la vue directement.
 */
class Game
{
    private:
        Player*   player;         ///< Joueur humain
        Campaign* campaign;       ///< Données de la campagne
        Settings* settings;       ///< Paramètres du jeu
        Shop*     shop;           ///< Boutique
        Battle*   currentBattle;  ///< Combat en cours (nullptr si aucun)

        GameState   state;          ///< État courant du jeu
        int         selectedIndex;  ///< Index sélectionné dans les listes
        int         saveSlot;       ///< Emplacement de sauvegarde actif (1-3)
        std::string statusMsg;      ///< Message de retour affiché une frame (feedback utilisateur)

    public:
        /// @brief Construit le jeu et charge les personnages depuis Characters.csv
        Game(void);
        ~Game(void);

        /// @brief Initialise le jeu (demande pseudo, propose chargement de sauvegarde)
        void init(void);

        /// @brief Met à jour le modèle en réponse à un événement
        void update(Event* ev);

        // Accesseurs
        Player*   getPlayer()        const;
        Campaign* getCampaign()      const;
        Settings* getSettings()      const;
        Shop*     getShop()          const;
        Battle*   getCurrentBattle() const;
        GameState   getState()         const;
        int         getSelectedIndex() const;
        void        setSelectedIndex(int i);
        std::string getStatusMsg()     const;
        void        clearStatusMsg();

        // Navigation dans les listes
        void moveSelectionUp();
        void moveSelectionDown();
        void confirmSelection();

        // Entrées clavier (selon l'état)
        EventType input(char c);
        EventType inputSaveSelect(char c);
        EventType inputMainMenu(char c);
        EventType inputSettings(char c);
        EventType inputPlayMenu(char c);
        EventType inputBank(char c);
        EventType inputBankDetail(char c);
        EventType inputCampaign(char c);
        EventType inputCampaignArc(char c);
        EventType inputBattlePrepa(char c);
        EventType inputBattle(char c);
        EventType inputTeam(char c);
        EventType inputTeamChange(char c);
        EventType inputShop(char c);

        // Sauvegarde
        void save();
        bool load();

        /// @brief Applique les récompenses après victoire (appelé par la vue)
        void applyRewards();

    private:
        /// @brief Démarre un nouveau combat pour l'épisode sélectionné
        void startBattle();
        /// @brief Génère l'équipe ennemie selon la difficulté de l'épisode
        void buildEnemyTeam(Player* enemy, int difficulty);
};

#endif
