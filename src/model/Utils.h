/**
 * @file Utils.h
 * @brief Énumérations et fonctions utilitaires partagées par tous les modules.
 */

#ifndef UTILS_H
#define UTILS_H

#include <string>

// Raretés

/// @brief Rareté d'un personnage (du plus commun au mythique)
enum class Rarity
{
    c,   ///< Commun
    r,   ///< Rare
    sr,  ///< Super Rare
    e,   ///< Épique
    l,   ///< Légendaire
    m    ///< Mythique
};

// Types de personnage

/// @brief Type de rôle d'un personnage en combat
enum class Type_
{
    snp_,  ///< Sniper
    int_,  ///< Intelligent
    mag_,  ///< Magicien
    atk_,  ///< Attaquant
    def_,  ///< Défenseur
    sup_   ///< Support
};

// Types d'effet

/// @brief Type d'effet appliqué par une capacité
enum class EffectType
{
    Damage,   ///< Inflige des dégâts
    Heal,     ///< Soigne
    Buff,     ///< Augmente une stat
    Debuff,   ///< Réduit une stat
    Resist,   ///< Réduit les dégâts reçus
    Stun,     ///< Empêche d'agir ce tour
    Swap,     ///< Échange deux personnages de position
    Bleeding, ///< Saignement (dégâts sur plusieurs tours)
    Push,     ///< Pousse la cible
    Pull,     ///< Attire la cible
    Other     ///< Effet spécial non classifié
};

// Types de cible

/// @brief Portée de la cible d'une capacité
enum class TargetType
{
    Self,       ///< Soi-même
    Ally,       ///< Un allié aléatoire
    Enemy,      ///< Un ennemi aléatoire
    AllAllies,  ///< Tous les alliés
    AllEnemies, ///< Tous les ennemis
    All         ///< Tout le monde
};

// Types de capacité

/// @brief Catégorie d'une capacité (pour l'affichage et les synergies)
enum class TypeC
{
    heal,        ///< Soin
    proj,        ///< Projectile
    fire,        ///< Feu
    light,       ///< Lumière
    sup,         ///< Support
    blade,       ///< Lame
    punch,       ///< Coup de poing
    air,         ///< Air / Vent
    ice,         ///< Glace
    kick,        ///< Coup de pied
    electricity, ///< Électricité
    water,       ///< Eau
    magic,       ///< Magie
    self_buff,   ///< Buff personnel
    other        ///< Autre
};

// Événements

/// @brief Types d'événements émis par les contrôleurs
enum class EventType
{
    MAIN_MENU,         ///< Retourner au menu principal
    SETTINGS,          ///< Ouvrir les paramètres
    PLAY_MENU,         ///< Entrer dans le menu de jeu
    BANK,              ///< Ouvrir la bank
    BANK_DETAIL,       ///< Voir le détail d'un personnage
    BANK_UPGRADE,      ///< Améliorer un personnage
    BANK_UNLOCK,       ///< Débloquer un personnage
    CAMPAIGN,          ///< Ouvrir la campagne
    CAMPAIGN_ARC,      ///< Sélectionner un arc
    CAMPAIGN_EPISODE,  ///< Sélectionner un épisode
    PLAY,              ///< Lancer la préparation de combat
    BATTLE,            ///< Démarrer le combat
    TEAM,              ///< Afficher les équipes
    TEAM_CHANGE,       ///< Modifier l'équipe joueur
    SHOP,              ///< Ouvrir le shop
    SAVE,              ///< Sauvegarder
    LOAD,              ///< Charger une sauvegarde
    BATTLE_RETURN,     ///< Retour à l'arc après combat (sans reset)
    BATTLE_RETRY,      ///< Rejouer le même épisode
    BATTLE_CONTINUE,   ///< Continuer vers l'épisode suivant
    SAVE_NEW,          ///< Démarrer une nouvelle partie
    SAVE_LOAD_1,       ///< Charger emplacement 1
    SAVE_LOAD_2,       ///< Charger emplacement 2
    SAVE_LOAD_3,       ///< Charger emplacement 3
    SAVE_SELECT,       ///< Afficher l'écran de sélection de sauvegarde
    QUIT,              ///< Quitter le jeu
    NONE               ///< Aucun événement
};

// États du jeu

/// @brief État courant du jeu (détermine quel écran est affiché)
enum class GameState
{
    SAVE_SELECT,   ///< Sélection de la sauvegarde (écran de démarrage)
    MAIN_MENU,     ///< Menu principal (Jouer / Paramètres / Quitter)
    SETTINGS,      ///< Écran des paramètres
    PLAY_MENU,     ///< Menu en jeu (Bank / Campagne / Shop)
    BANK,          ///< Liste des personnages
    BANK_DETAIL,   ///< Détail d'un personnage
    CAMPAIGN,      ///< Liste des arcs
    CAMPAIGN_ARC,  ///< Épisodes d'un arc sélectionné
    BATTLE_PREPA,  ///< Préparation du combat (choix équipe)
    BATTLE,        ///< Combat en cours
    TEAM,          ///< Affichage des équipes
    TEAM_CHANGE,   ///< Modification de l'équipe
    SHOP,          ///< Écran du shop
    QUIT           ///< Quitter
};

// Structure utilitaire

/// @brief Paire de types de capacité
struct TupleTC
{
    TypeC t1; ///< Type principal
    TypeC t2; ///< Type secondaire
};

// Conversions

Type_      stringToType_      (const std::string& str);
Rarity     stringToRarity     (const std::string& str);
TypeC      stringToTypeC      (const std::string& s);
TargetType stringToTargetType (const std::string& s);
EffectType stringToEffect     (const std::string& s);

std::string Type_Tostring      (const Type_& typ);
std::string Type_DisplayString (const Type_& typ);
std::string RarityTostring     (const Rarity& rar);
std::string Rarity_DisplayString(const Rarity& rar);
std::string typeCToString      (TypeC& t);
std::string targetTypeToString (TargetType t);
std::string EffectTostring     (EffectType e);

// Divers

/// @brief Supprime les espaces et \r en début/fin de chaîne
std::string clean(std::string s);

/// @brief Compte le nombre de lignes dans un flux ouvert (remet le curseur au début)
int countLines(std::ifstream& file);

#endif
