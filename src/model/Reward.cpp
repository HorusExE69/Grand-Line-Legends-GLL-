/**
 * @file Reward.cpp
 * @brief Implémentation de Reward.
 */

#include "Reward.h"

using namespace std;

Reward::Reward(int b, const string& charName, int frags, float chance)
{
    berries          = b;
    fragmentCharName = charName;
    nbFragments      = frags;
    dropChance       = chance;
}

int    Reward::getBerries()          const { return berries;          }
string Reward::getFragmentCharName() const { return fragmentCharName; }
int    Reward::getNbFragments()      const { return nbFragments;      }
float  Reward::getDropChance()       const { return dropChance;       }
