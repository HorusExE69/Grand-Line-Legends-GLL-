/**
 * @file Shop.cpp
 * @brief Implémentation de Shop.
 */

#include "Shop.h"
#include <ctime>
#include <cassert>

using namespace std;

// Données statiques des offres possibles
static const char* DAILY_CHARS[7] = {
    "Luffy", "Zoro", "Nami", "Sanji", "Ace", "Shanks", "Marco"
};
static const int DAILY_COSTS[7] = { 500, 600, 500, 600, 800, 1500, 1200 };

static const char* WEEKLY_CHARS[4] = { "Ace", "BarbeBlanche", "Akainu", "Shanks" };
static const int   WEEKLY_FRAGS[4] = { 20, 15, 20, 15 };
static const int   WEEKLY_COSTS[4] = { 2000, 3000, 2500, 3500 };

static const char* MONTHLY_CHARS[12] = {
    "BarbeBlanche", "BarbeNoire", "Akainu", "Shanks", "Marco",
    "Ace", "Dragon", "Garp", "Sengoku", "Robin", "Zoro", "Sanji"
};
static const int MONTHLY_COSTS[12] = {
    8000, 8000, 8000, 10000, 7000,
    7000, 9000, 7500, 8500, 6000, 6500, 7000
};

void Shop::buildDailyOffer(int dayOfWeek)
{
    assert(dayOfWeek >= 0 && dayOfWeek <= 6);

    daily.label       = string("Fragments de ") + DAILY_CHARS[dayOfWeek];
    daily.costBerries = DAILY_COSTS[dayOfWeek];
    daily.nbFragments = 10;
    daily.charName    = DAILY_CHARS[dayOfWeek];
    daily.isCoffre    = false;
    daily.coffreRar   = Rarity::c;
}

void Shop::buildWeeklyOffer(int weekOfMonth)
{
    assert(weekOfMonth >= 0 && weekOfMonth <= 3);

    weekly.label       = string("Pack ") + WEEKLY_CHARS[weekOfMonth];
    weekly.costBerries = WEEKLY_COSTS[weekOfMonth];
    weekly.nbFragments = WEEKLY_FRAGS[weekOfMonth];
    weekly.charName    = WEEKLY_CHARS[weekOfMonth];
    weekly.isCoffre    = false;
    weekly.coffreRar   = Rarity::c;
}

void Shop::buildMonthlyOffer(int month)
{
    assert(month >= 1 && month <= 12);

    int idx = month - 1;
    monthly.label       = string("Offre Mensuelle : ") + MONTHLY_CHARS[idx];
    monthly.costBerries = MONTHLY_COSTS[idx];
    monthly.nbFragments = 50;
    monthly.charName    = MONTHLY_CHARS[idx];
    monthly.isCoffre    = false;
    monthly.coffreRar   = Rarity::c;
}

void Shop::buildPermanentOffers()
{
    // Offre 1 : Coffre Rare
    permanent[0].label       = "Coffre Rare";
    permanent[0].costBerries = 2000;
    permanent[0].nbFragments = 0;
    permanent[0].charName    = "";
    permanent[0].isCoffre    = true;
    permanent[0].coffreRar   = Rarity::r;

    // Offre 2 : Coffre Super Rare
    permanent[1].label       = "Coffre Super Rare";
    permanent[1].costBerries = 6000;
    permanent[1].nbFragments = 0;
    permanent[1].charName    = "";
    permanent[1].isCoffre    = true;
    permanent[1].coffreRar   = Rarity::sr;

    // Offre 3 : Coffre Epique
    permanent[2].label       = "Coffre Epique";
    permanent[2].costBerries = 15000;
    permanent[2].nbFragments = 0;
    permanent[2].charName    = "";
    permanent[2].isCoffre    = true;
    permanent[2].coffreRar   = Rarity::e;

    // Offre 4 : Coffre Legendaire
    permanent[3].label       = "Coffre Legendaire";
    permanent[3].costBerries = 40000;
    permanent[3].nbFragments = 0;
    permanent[3].charName    = "";
    permanent[3].isCoffre    = true;
    permanent[3].coffreRar   = Rarity::l;

    // Offre 5 : Coffre Mythique
    permanent[4].label       = "Coffre Mythique";
    permanent[4].costBerries = 100000;
    permanent[4].nbFragments = 0;
    permanent[4].charName    = "";
    permanent[4].isCoffre    = true;
    permanent[4].coffreRar   = Rarity::m;
}

Shop::Shop()
{
    // Récupérer la date système (pour les offres daily/weekly/monthly si besoin futur)
    time_t     now = time(nullptr);
    struct tm* t   = localtime(&now);

    int dayOfWeek   = t->tm_wday;
    int weekOfMonth = (t->tm_mday - 1) / 7;
    int month       = t->tm_mon + 1;

    // Initialiser les offres daily/weekly/monthly (non affichées mais membres valides)
    buildDailyOffer(dayOfWeek);
    buildWeeklyOffer(weekOfMonth);
    buildMonthlyOffer(month);
    buildPermanentOffers();
}

const ShopOffer& Shop::getDaily()               const { return daily;        }
const ShopOffer& Shop::getWeekly()              const { return weekly;       }
const ShopOffer& Shop::getMonthly()             const { return monthly;      }
const ShopOffer& Shop::getPermanent(int index)  const
{
    assert(index >= 0 && index < NB_SHOP_PERMANENT);
    return permanent[index];
}
