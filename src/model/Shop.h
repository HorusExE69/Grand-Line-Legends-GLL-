/**
 * @file Shop.h
 * @brief Boutique du jeu avec offres journalières, hebdomadaires et mensuelles.
 */

#ifndef SHOP_H
#define SHOP_H

#include <string>
#include "Coffre.h"

/// @brief Nombre d'offres permanentes dans le shop
static const int NB_SHOP_PERMANENT = 5;

/**
 * @class ShopOffer
 * @brief Représente une offre disponible dans le shop.
 */
struct ShopOffer
{
    std::string label;       ///< Description de l'offre
    int         costBerries; ///< Coût en berries
    int         nbFragments; ///< Fragments accordés (si applicable)
    std::string charName;    ///< Nom du personnage ciblé (si applicable)
    Rarity      coffreRar;   ///< Rareté du coffre offert
    bool        isCoffre;    ///< Vrai si l'offre donne un coffre
};

/**
 * @class Shop
 * @brief Gère l'ensemble des offres de la boutique.
 *
 * Les offres sont générées selon le jour de la semaine (offre journalière),
 * la semaine du mois (offre hebdo) et le mois (offre mensuelle).
 */
class Shop
{
    private:
        ShopOffer daily;              ///< Offre du jour
        ShopOffer weekly;             ///< Offre de la semaine
        ShopOffer monthly;            ///< Offre du mois
        ShopOffer permanent[NB_SHOP_PERMANENT]; ///< Offres permanentes

        /// @brief Génère l'offre journalière selon le jour (0-6)
        void buildDailyOffer(int dayOfWeek);
        /// @brief Génère l'offre hebdomadaire selon la semaine (0-3)
        void buildWeeklyOffer(int weekOfMonth);
        /// @brief Génère l'offre mensuelle selon le mois (1-12)
        void buildMonthlyOffer(int month);
        /// @brief Initialise les offres permanentes
        void buildPermanentOffers();

    public:
        /// @brief Constructeur : génère les offres selon la date système
        Shop();

        // Accesseurs
        const ShopOffer& getDaily()               const;
        const ShopOffer& getWeekly()              const;
        const ShopOffer& getMonthly()             const;
        const ShopOffer& getPermanent(int index)  const;
};

#endif
