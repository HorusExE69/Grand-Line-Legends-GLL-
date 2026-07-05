/**
 * @file Episode.cpp
 * @brief Implémentation de Episode.
 */

#include "Episode.h"
#include <cassert>

using namespace std;

Episode::Episode(const string& n, int diff, bool miniBoss, bool boss)
{
    assert(diff >= 0);

    name      = n;
    difficulty = diff;
    isMiniBoss = miniBoss;
    isBoss     = boss;
    completed  = false;
    rewards    = nullptr;
    nbRewards  = 0;
}

Episode::~Episode()
{
    for (int i = 0; i < nbRewards; i++)
        delete rewards[i];
    delete[] rewards;
}

// Accesseurs
const string& Episode::getName()        const { return name;              }
int           Episode::getDifficulty()  const { return difficulty;        }
bool          Episode::getIsMiniBoss()  const { return isMiniBoss;        }
bool          Episode::getIsBoss()      const { return isBoss;            }
bool          Episode::isCompleted()    const { return completed;         }
int           Episode::getNbRewards()   const { return nbRewards;         }
Reward*       Episode::getReward(int i) const { return rewards[i];        }

void          Episode::setCompleted(bool val) { completed = val;          }

// Ajoute une récompense au tableau (redimensionnement dynamique)
void Episode::addReward(Reward* r)
{
    assert(r != nullptr);

    Reward** tmp = new Reward*[nbRewards + 1];
    for (int i = 0; i < nbRewards; i++)
        tmp[i] = rewards[i];
    tmp[nbRewards] = r;
    delete[] rewards;
    rewards = tmp;
    nbRewards++;
}
