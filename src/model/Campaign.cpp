/**
 * @file Campaign.cpp
 * @brief Implémentation de Campaign et définition des 17 arcs.
 */

#include "Campaign.h"
#include <cassert>

// Macros locales

// Crée un épisode "sbires" (combat normal)
static Episode* makeEp(const char* name, int diff)
{
    return new Episode(name, diff, false, false);
}
// Crée un épisode "mini-boss"
static Episode* makeMiniBoss(const char* name, int diff)
{
    return new Episode(name, diff, true, false);
}
// Crée un épisode "boss final"
static Episode* makeBoss(const char* name, int diff)
{
    return new Episode(name, diff, false, true);
}

// Ajout des récompenses

static void addBerryReward(Episode* ep, int berries)
{
    ep->addReward(new Reward(berries, "", 0, 1.0f));
}

static void addFragReward(Episode* ep, const char* charName, int frags, float chance)
{
    ep->addReward(new Reward(0, charName, frags, chance));
}

// Construction des arcs

void Campaign::buildArcs()
{
    int i = 0;

    // Arc 0 : Romance Dawn — Boss : Morgan — Unlock : Zoro 100%
    arcs[i] = new Arc("Romance Dawn", "Morgan", "Zoro", 1.0f);
    arcs[i]->addEpisode(makeEp("Sbires de la Marine", 1));
    arcs[i]->addEpisode(makeEp("Garde de Morgan", 2));
    arcs[i]->addEpisode(makeEp("Officiers Marines", 3));
    arcs[i]->addEpisode(makeMiniBoss("Helmeppo", 4));
    arcs[i]->addEpisode(makeBoss("Capitaine Morgan", 5));
    addBerryReward(arcs[i]->episodes[4], 1000);
    addFragReward (arcs[i]->episodes[4], "Zoro", 100, 1.0f);
    i++;

    // Arc 1 : Orange Town — Boss : Baggy — Unlock : Nami 50%
    arcs[i] = new Arc("Orange Town", "Baggy", "Nami", 0.5f);
    arcs[i]->addBanned("Baggy");  // Baggy est le boss, interdit côté joueur
    arcs[i]->addEpisode(makeEp("Pirates de Baggy", 2));
    arcs[i]->addEpisode(makeEp("Clown Gang", 3));
    arcs[i]->addEpisode(makeEp("Lieutenants de Baggy", 4));
    arcs[i]->addEpisode(makeMiniBoss("Mohji", 4));
    arcs[i]->addEpisode(makeBoss("Baggy le Clown", 6));
    addBerryReward(arcs[i]->episodes[4], 1500);
    addFragReward (arcs[i]->episodes[4], "Nami", 50, 1.0f);
    i++;

    // Arc 2 : Village Sirop — Boss : Kuro — Unlock : Usopp 100%
    arcs[i] = new Arc("Village Sirop", "Kuro", "Usopp", 1.0f);
    arcs[i]->addEpisode(makeEp("Pirates Chats", 2));
    arcs[i]->addEpisode(makeEp("Sham & Buchi", 3));
    arcs[i]->addEpisode(makeEp("Gardes de Kuro", 4));
    arcs[i]->addEpisode(makeMiniBoss("Jango", 5));
    arcs[i]->addEpisode(makeBoss("Kuro", 7));
    addBerryReward(arcs[i]->episodes[4], 2000);
    addFragReward (arcs[i]->episodes[4], "Usopp", 100, 1.0f);
    i++;

    // Arc 3 : Baratie — Boss : Don Zrieg — Unlock : Sanji 100%
    arcs[i] = new Arc("Baratie", "Don Zrieg", "Sanji", 1.0f);
    arcs[i]->addEpisode(makeEp("Corsaires de Zrieg", 3));
    arcs[i]->addEpisode(makeEp("Officiers", 4));
    arcs[i]->addEpisode(makeEp("Flotte de Zrieg", 5));
    arcs[i]->addEpisode(makeMiniBoss("Gin", 6));
    arcs[i]->addEpisode(makeBoss("Don Zrieg", 8));
    addBerryReward(arcs[i]->episodes[4], 2500);
    addFragReward (arcs[i]->episodes[4], "Sanji", 100, 1.0f);
    i++;

    // Arc 4 : Arlong Park — Boss : Arlong — Unlock : Nami 50%
    arcs[i] = new Arc("Arlong Park", "Arlong", "Nami", 0.5f);
    arcs[i]->addBanned("Nami");   // Nami est prisonnière d'Arlong ici
    arcs[i]->addEpisode(makeEp("Pirates Poissons-Hommes", 3));
    arcs[i]->addEpisode(makeEp("Gardes d'Arlong", 5));
    arcs[i]->addEpisode(makeEp("Élite d'Arlong Park", 6));
    arcs[i]->addEpisode(makeMiniBoss("Hachi", 7));
    arcs[i]->addEpisode(makeBoss("Arlong", 9));
    addBerryReward(arcs[i]->episodes[4], 3000);
    addFragReward (arcs[i]->episodes[4], "Nami", 50, 1.0f);
    i++;

    // Arc 5 : Logue Town — Boss : Smoker — Unlock : Baggy 100%
    arcs[i] = new Arc("Logue Town", "Smoker", "Baggy", 1.0f);
    arcs[i]->addBanned("Smoker"); // Smoker est l'antagoniste de cet arc
    arcs[i]->addEpisode(makeEp("Marines de Logue Town", 4));
    arcs[i]->addEpisode(makeEp("Patrouilles", 5));
    arcs[i]->addEpisode(makeEp("Élite Marine", 7));
    arcs[i]->addEpisode(makeMiniBoss("Tashigi", 8));
    arcs[i]->addEpisode(makeBoss("Smoker", 10));
    addBerryReward(arcs[i]->episodes[4], 4000);
    addFragReward (arcs[i]->episodes[4], "Baggy", 100, 1.0f);
    i++;

    // Arc 6 : Whiskey Peak — Boss : Mr5 + Miss Valentine — Unlock : Vivi 100%
    arcs[i] = new Arc("Whiskey Peak", "Mr5 & Miss Valentine", "Vivi", 1.0f);
    arcs[i]->addEpisode(makeEp("Agents Baroque Works", 5));
    arcs[i]->addEpisode(makeEp("Mr9 & Miss Wednesday", 6));
    arcs[i]->addEpisode(makeEp("Agents de rang S", 7));
    arcs[i]->addEpisode(makeMiniBoss("Mr9", 8));
    arcs[i]->addEpisode(makeBoss("Mr5 & Miss Valentine", 11));
    addBerryReward(arcs[i]->episodes[4], 5000);
    addFragReward (arcs[i]->episodes[4], "Vivi", 100, 1.0f);
    i++;

    // Arc 7 : Little Garden — Boss : Mr3 + Miss GW — Unlock : DoggyBroggy 50%
    arcs[i] = new Arc("Little Garden", "Mr3 & Miss GW", "DoggyBroggy", 0.5f);
    arcs[i]->addEpisode(makeEp("Créatures de l'île", 5));
    arcs[i]->addEpisode(makeEp("Agents BW rang A", 6));
    arcs[i]->addEpisode(makeEp("Pièges de cire", 7));
    arcs[i]->addEpisode(makeMiniBoss("Miss Goldenweek", 8));
    arcs[i]->addEpisode(makeBoss("Mr3", 12));
    addBerryReward(arcs[i]->episodes[4], 5500);
    addFragReward (arcs[i]->episodes[4], "DoggyBroggy", 50, 1.0f);
    i++;

    // Arc 8 : Drum Island — Boss : Wapol — Unlock : Chopper 100% (Nami bannie)
    arcs[i] = new Arc("Drum Island", "Wapol", "Chopper", 1.0f);
    arcs[i]->addBanned("Nami");
    arcs[i]->addEpisode(makeEp("Soldats de Wapol", 6));
    arcs[i]->addEpisode(makeEp("Chess & Kuromarimo", 7));
    arcs[i]->addEpisode(makeEp("Garde royale", 8));
    arcs[i]->addEpisode(makeMiniBoss("Chess", 9));
    arcs[i]->addEpisode(makeBoss("Wapol", 13));
    addBerryReward(arcs[i]->episodes[4], 6000);
    addFragReward (arcs[i]->episodes[4], "Chopper", 100, 1.0f);
    i++;

    // Arc 9 : Alabasta — Boss : Crocodile — Unlock : Robin 100% + Ponéglyph 2
    arcs[i] = new Arc("Alabasta", "Crocodile", "Robin", 1.0f);
    arcs[i]->addEpisode(makeEp("Agents BW Officiers", 7));
    arcs[i]->addEpisode(makeEp("Mr2 Bon Clay", 8));
    arcs[i]->addEpisode(makeEp("Mr1 Daz Bones", 9));
    arcs[i]->addEpisode(makeMiniBoss("Mr1", 11));
    arcs[i]->addEpisode(makeBoss("Crocodile", 15));
    addBerryReward(arcs[i]->episodes[4], 8000);
    addFragReward (arcs[i]->episodes[4], "Robin", 100, 1.0f);
    i++;

    // Arc 10 : Skypea — Boss : Ener — Ponéglyph 1
    arcs[i] = new Arc("Skypea", "Ener", "", 0.0f);
    arcs[i]->addEpisode(makeEp("Gardes Shandora", 7));
    arcs[i]->addEpisode(makeEp("Prêtres d'Ener", 8));
    arcs[i]->addEpisode(makeEp("Shandians", 9));
    arcs[i]->addEpisode(makeMiniBoss("Ohm", 11));
    arcs[i]->addEpisode(makeBoss("Ener", 16));
    addBerryReward(arcs[i]->episodes[4], 9000);
    i++;

    // Arc 11 : Water Seven — Boss : Franky — Unlock : Franky 50% (Usopp, Robin, Franky bannis)
    arcs[i] = new Arc("Water Seven", "Franky", "Franky", 0.5f);
    arcs[i]->addBanned("Usopp");
    arcs[i]->addBanned("Robin");
    arcs[i]->addBanned("Franky"); // Franky est le boss de cet arc
    arcs[i]->addEpisode(makeEp("Franky Family", 8));
    arcs[i]->addEpisode(makeEp("CP9 Agents", 9));
    arcs[i]->addEpisode(makeEp("Élite CP9", 10));
    arcs[i]->addEpisode(makeMiniBoss("Blueno", 11));
    arcs[i]->addEpisode(makeBoss("Franky", 17));
    addBerryReward(arcs[i]->episodes[4], 10000);
    addFragReward (arcs[i]->episodes[4], "Franky", 50, 1.0f);
    i++;

    // Arc 12 : Enies Lobby — Boss : Rob Lucci — Unlock : Franky 50% + skin Usopp (Robin bannie)
    arcs[i] = new Arc("Enies Lobby", "Rob Lucci", "Franky", 0.5f);
    arcs[i]->addBanned("Robin");
    arcs[i]->addEpisode(makeEp("Marines d'Enies Lobby", 9));
    arcs[i]->addEpisode(makeEp("CP9 Élite", 10));
    arcs[i]->addEpisode(makeEp("Kaku & Jabra", 11));
    arcs[i]->addEpisode(makeMiniBoss("Kaku", 13));
    arcs[i]->addEpisode(makeBoss("Rob Lucci", 18));
    addBerryReward(arcs[i]->episodes[4], 12000);
    addFragReward (arcs[i]->episodes[4], "Franky", 60, 1.0f);
    i++;

    // Arc 13 : Thriller Bark — Boss : Gecko Moria — Unlock : Brook 100%
    arcs[i] = new Arc("Thriller Bark", "Gecko Moria", "Brook", 1.0f);
    arcs[i]->addEpisode(makeEp("Zombies de Moria", 9));
    arcs[i]->addEpisode(makeEp("Soldats-Zombies", 10));
    arcs[i]->addEpisode(makeEp("Absalom & Perona", 11));
    arcs[i]->addEpisode(makeMiniBoss("Perona", 13));
    arcs[i]->addEpisode(makeBoss("Gecko Moria", 19));
    addBerryReward(arcs[i]->episodes[4], 14000);
    addFragReward (arcs[i]->episodes[4], "Brook", 100, 1.0f);
    i++;

    // Arc 14 : Amazon Lily — Boss : Boa Hancock — Unlock : Boa Hancock 100%
    arcs[i] = new Arc("Amazon Lily", "Boa Hancock", "Boa Hancock", 1.0f);
    arcs[i]->addEpisode(makeEp("Kuja Warriors", 10));
    arcs[i]->addEpisode(makeEp("Sœurs Boa", 11));
    arcs[i]->addEpisode(makeEp("Gardes Kuja", 12));
    arcs[i]->addEpisode(makeMiniBoss("Sandersonia", 13));
    arcs[i]->addEpisode(makeBoss("Boa Hancock", 20));
    addBerryReward(arcs[i]->episodes[4], 16000);
    addFragReward (arcs[i]->episodes[4], "Boa Hancock", 100, 1.0f);
    i++;

    // Arc 15 : Impel Down — Boss : Magellan — Unlocks multiples
    arcs[i] = new Arc("Impel Down", "Magellan", "Bon Clay", 1.0f);
    arcs[i]->addEpisode(makeEp("Gardes Niveau 1-2", 11));
    arcs[i]->addEpisode(makeEp("Bêtes du Niveau 3", 12));
    arcs[i]->addEpisode(makeEp("Gardes Niveau 4-5", 13));
    arcs[i]->addEpisode(makeMiniBoss("Saldeath", 14));
    arcs[i]->addEpisode(makeBoss("Magellan", 21));
    addBerryReward(arcs[i]->episodes[4], 18000);
    addFragReward (arcs[i]->episodes[4], "Bon Clay",  100, 1.0f);
    addFragReward (arcs[i]->episodes[4], "Crocodile",  25, 0.25f);
    addFragReward (arcs[i]->episodes[4], "Jinbe",      25, 0.25f);
    addFragReward (arcs[i]->episodes[4], "Ivankov",    25, 0.25f);
    i++;

    // Arc 16 : Marine Ford — Boss : Akainu — Amiraux, Sengoku — Unlocks multiples + Mythic
    arcs[i] = new Arc("Marine Ford", "Akainu le Poing de Justice", "Ace", 0.5f);
    arcs[i]->addEpisode(makeEp("Avance vers Marineford", 17));
    arcs[i]->addEpisode(makeEp("Kizaru entre en scene", 21));
    arcs[i]->addEpisode(makeMiniBoss("Aokiji - Le Pouvoir de la Glace", 26));
    arcs[i]->addEpisode(makeMiniBoss("Sengoku - Le Grand Bouddha", 31));
    arcs[i]->addEpisode(makeBoss("Akainu - Le Poing de la Justice", 37));
    addBerryReward(arcs[i]->episodes[4], 50000);
    addFragReward (arcs[i]->episodes[4], "Ace",          50, 1.0f);
    addFragReward (arcs[i]->episodes[4], "Crocodile",    25, 0.25f);
    addFragReward (arcs[i]->episodes[4], "Ivankov",      25, 0.25f);
    addFragReward (arcs[i]->episodes[4], "Jinbe",        25, 0.25f);
    addFragReward (arcs[i]->episodes[4], "BarbeBlanche", 25, 0.25f);
    addFragReward (arcs[i]->episodes[4], "Marco",        25, 0.25f);
    addFragReward (arcs[i]->episodes[2], "Aokiji",  30, 0.5f);
    addFragReward (arcs[i]->episodes[3], "Sengoku", 30, 0.5f);
    i++;

    // Arc 17 : Pantheon — Boss infiniment difficiles, débloqué après Marine Ford
    arcs[i] = new Arc("Pantheon", "Tous les Boss", "", 0.0f);
    arcs[i]->addEpisode(makeBoss("Etage 1 - Morgan revient", 25));
    arcs[i]->addEpisode(makeBoss("Etage 2 - Baggy supreme", 28));
    arcs[i]->addEpisode(makeBoss("Etage 3 - Kuro l'impitoyable", 32));
    arcs[i]->addEpisode(makeBoss("Etage 4 - Arlong furieux", 37));
    arcs[i]->addEpisode(makeBoss("Etage 5 - Crocodile ultime", 43));
    arcs[i]->addEpisode(makeBoss("Etage 6 - Lucci sans pitie", 50));
    arcs[i]->addEpisode(makeBoss("Etage 7 - Moria omnipotent", 58));
    arcs[i]->addEpisode(makeBoss("Etage 8 - Hancock implacable", 67));
    arcs[i]->addEpisode(makeBoss("Etage 9 - Magellan supreme", 77));
    arcs[i]->addEpisode(makeBoss("Etage 10 - AKAINU FINAL", 88));
    // Récompenses massives pour chaque étage
    for (int floor = 0; floor < 10; floor++) {
        addBerryReward(arcs[i]->episodes[floor], 10000 * (floor + 1));
        addFragReward(arcs[i]->episodes[floor], "Shanks",       30, 0.5f);
        addFragReward(arcs[i]->episodes[floor], "BarbeBlanche", 30, 0.3f);
        addFragReward(arcs[i]->episodes[floor], "Kaido",        20, 0.25f);
    }
    i++;
}

// Constructeur / Destructeur

Campaign::Campaign()
{
    arcs       = new Arc*[NB_ARCS];
    currentArc = 0;
    currentEp  = 0;
    buildArcs();
}

Campaign::~Campaign()
{
    for (int i = 0; i < NB_ARCS; i++)
        delete arcs[i];
    delete[] arcs;
}

// Navigation

void Campaign::selectArc(int idx)
{
    assert(idx >= 0 && idx < NB_ARCS);
    currentArc = idx;
    currentEp  = 0;
}

void Campaign::selectEpisode(int idx)
{
    assert(idx >= 0 && idx < arcs[currentArc]->nbEpisodes);
    currentEp = idx;
}

void Campaign::arcUp()
{
    if (currentArc > 0) currentArc--;
}

void Campaign::arcDown()
{
    if (currentArc < NB_ARCS - 1) currentArc++;
}

void Campaign::episodeUp()
{
    if (currentEp > 0) currentEp--;
}

void Campaign::episodeDown()
{
    if (currentEp < arcs[currentArc]->nbEpisodes - 1) currentEp++;
}

// Accesseurs

Arc* Campaign::getArc(int idx) const
{
    assert(idx >= 0 && idx < NB_ARCS);
    return arcs[idx];
}

Arc* Campaign::getCurrentArc() const { return arcs[currentArc]; }
int  Campaign::getCurrentArcIdx() const { return currentArc;    }
int  Campaign::getCurrentEpIdx()  const { return currentEp;     }
int  Campaign::getNbArcs()        const { return NB_ARCS;       }

Episode* Campaign::getCurrentEpisode() const
{
    return arcs[currentArc]->episodes[currentEp];
}

void Campaign::completeCurrentEpisode()
{
    arcs[currentArc]->episodes[currentEp]->completed = true;
    checkArcCompletion();
}

void Campaign::checkArcCompletion()
{
    Arc* arc = arcs[currentArc];
    for (int i = 0; i < arc->nbEpisodes; i++)
    {
        if (!arc->episodes[i]->completed) return;
    }
    arc->completed = true;
}
