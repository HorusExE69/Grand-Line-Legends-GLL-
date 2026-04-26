/**
 * @file Arc.cpp
 * @brief Implémentation de Arc.
 */

#include "Arc.h"
#include <cassert>

using namespace std;

Arc::Arc(const string& arcName, const string& boss,
         const string& unlock, float chance)
{
    assert(chance >= 0.0f && chance <= 1.0f);

    name           = arcName;
    bossName       = boss;
    unlockCharName = unlock;
    unlockChance   = chance;
    nbBanned       = 0;
    completed      = false;

    maxEpisodes = 8;
    nbEpisodes  = 0;
    episodes    = new Episode*[maxEpisodes];
}

Arc::~Arc()
{
    for (int i = 0; i < nbEpisodes; i++)
        delete episodes[i];
    delete[] episodes;
}

// Ajoute un épisode avec redimensionnement dynamique
void Arc::addEpisode(Episode* ep)
{
    assert(ep != nullptr);

    if (nbEpisodes >= maxEpisodes)
    {
        int newMax          = maxEpisodes * 2;
        Episode** newTab    = new Episode*[newMax];
        for (int i = 0; i < nbEpisodes; i++)
            newTab[i] = episodes[i];
        delete[] episodes;
        episodes    = newTab;
        maxEpisodes = newMax;
    }
    episodes[nbEpisodes++] = ep;
}

void Arc::addBanned(const string& charName)
{
    assert(nbBanned < MAX_BANNED);
    bannedChars[nbBanned++] = charName;
}

bool Arc::isBanned(const string& charName) const
{
    for (int i = 0; i < nbBanned; i++)
    {
        if (bannedChars[i] == charName)
            return true;
    }
    return false;
}
