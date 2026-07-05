/**
 * @file Player.cpp
 * @brief Implémentation de Player.
 */

#include "Player.h"
#include "Utils.h"
#include <fstream>
#include <cassert>
#include <cstdlib>

using namespace std;

// Constructeurs / Destructeur

Player::Player(const string& p)
{
    pseudo     = p;
    lvl        = 1;
    accountLvl = 1;
    xp         = 0;
    berries    = 500;

    bankMax = 4;
    nbBank  = 0;
    bank    = new Character*[bankMax];

    ulkMax   = 4;
    nbUnlock = 0;
    unlocked = new Character*[ulkMax];

    teamMax  = 1;
    teamSize = 0;
    team     = new Character*[teamMax];

    maxFrag  = 8;
    nbFrag   = 0;
    fragments = new FragEntry[maxFrag];
}

Player::Player(ifstream& file, const string& path, const string& p)
{
    pseudo     = p;
    lvl        = 1;
    accountLvl = 1;
    xp         = 0;
    berries    = 500;

    bankMax = countLines(file);
    if (bankMax < 4) bankMax = 4;
    nbBank  = 0;
    bank    = new Character*[bankMax];

    string line;
    getline(file, line); // en-tête

    while (getline(file, line))
    {
        if (line.empty()) continue;
        Character* c = new Character(line, path);
        addToBank(c);
    }

    ulkMax   = bankMax;
    nbUnlock = 0;
    unlocked = new Character*[ulkMax];

    teamMax  = 1;
    teamSize = 0;
    team     = new Character*[teamMax];

    maxFrag  = bankMax;
    nbFrag   = 0;
    fragments = new FragEntry[maxFrag];
}

Player::~Player()
{
    for (int i = 0; i < nbBank; i++)
        delete bank[i];
    delete[] bank;
    delete[] unlocked;
    delete[] team;
    delete[] fragments;
}

// Pseudo

void Player::changePseudo(const string& p) { pseudo = p; }

// Bank

void Player::addToBank(Character* c)
{
    assert(c != nullptr);

    if (nbBank >= bankMax)
    {
        int         newCap = bankMax * 2;
        Character** newTab = new Character*[newCap];
        for (int i = 0; i < nbBank; i++) newTab[i] = bank[i];
        delete[] bank;
        bank    = newTab;
        bankMax = newCap;
    }
    bank[nbBank++] = c;
}

Character* Player::getBankCharacter(int index) const
{
    assert(index >= 0 && index < nbBank);
    return bank[index];
}

int Player::getNbBank()  const { return nbBank;  }
int Player::getBankMax() const { return bankMax; }

// Débloqués

void Player::addToUnlocked(Character* c)
{
    assert(c != nullptr);
    if (isUnlocked(c->getName())) return;

    if (nbUnlock >= ulkMax)
    {
        int         newCap = ulkMax * 2;
        Character** newTab = new Character*[newCap];
        for (int i = 0; i < nbUnlock; i++) newTab[i] = unlocked[i];
        delete[] unlocked;
        unlocked = newTab;
        ulkMax   = newCap;
    }
    unlocked[nbUnlock++] = c;
}

bool Player::unlockCharacter(Character* c)
{
    assert(c != nullptr);
    if (getFragments(c->getName()) >= UNLOCK_FRAGMENT_COST)
    {
        addToUnlocked(c);
        // Déduire les fragments utilisés
        for (int i = 0; i < nbFrag; i++)
        {
            if (fragments[i].charName == c->getName())
            {
                fragments[i].count -= UNLOCK_FRAGMENT_COST;
                return true;
            }
        }
        return true;
    }
    return false;
}

void Player::unlockAll()
{
    for (int i = 0; i < nbBank; i++)
        addToUnlocked(bank[i]);
}

Character* Player::getUnlockCharacter(int index) const
{
    assert(index >= 0 && index < nbUnlock);
    return unlocked[index];
}

bool Player::isUnlocked(const string& charName) const
{
    for (int i = 0; i < nbUnlock; i++)
    {
        if (unlocked[i]->getName() == charName) return true;
    }
    return false;
}

int Player::getNbUnlock() const { return nbUnlock; }
int Player::getUlkMax()   const { return ulkMax;   }

// Équipe

bool Player::addToTeam(Character* c)
{
    assert(c != nullptr);
    if (teamSize >= teamMax) return false;
    // Vérifier que le personnage n'est pas déjà dans l'équipe
    for (int i = 0; i < teamSize; i++)
    {
        if (team[i] == c) return false;
    }
    team[teamSize++] = c;
    return true;
}

void Player::removeFromTeam(int index)
{
    assert(index >= 0 && index < teamSize);
    for (int i = index; i < teamSize - 1; i++)
        team[i] = team[i + 1];
    teamSize--;
}

void Player::addTeamSize(int nb)
{
    assert(nb > 0);
    int newMax = teamMax + nb;
    if (newMax > 10) newMax = 10; // maximum 10 slots

    Character** newTeam = new Character*[newMax];
    for (int i = 0; i < teamSize; i++) newTeam[i] = team[i];
    delete[] team;
    team    = newTeam;
    teamMax = newMax;
}

void Player::setTeamSlot(int slot, Character* c)
{
    assert(c != nullptr);
    assert(slot >= 0 && slot < teamMax);
    // Retirer le personnage s'il est déjà ailleurs dans l'équipe
    for (int i = 0; i < teamSize; i++)
    {
        if (team[i] == c && i != slot)
        {
            removeFromTeam(i);
            if (slot > i) slot--; // décaler l'index après la suppression
            break;
        }
    }
    if (slot < teamSize)
    {
        // Remplacer à la position existante
        team[slot] = c;
    }
    else
    {
        // Étendre avec des slots vides (nullptr) jusqu'au slot cible
        while (teamSize < slot && teamSize < teamMax)
        {
            team[teamSize] = nullptr;
            teamSize++;
        }
        if (teamSize < teamMax)
        {
            team[teamSize] = c;
            teamSize++;
        }
    }
}

void Player::clearTeam()
{
    teamSize = 0;
}

void Player::randomTeam()
{
    clearTeam();
    if (nbUnlock == 0) return;
    for (int i = 0; i < teamMax && i < nbUnlock; i++)
    {
        int idx = rand() % nbUnlock;
        addToTeam(unlocked[idx]);
    }
}

void Player::resetTeamPV()
{
    for (int i = 0; i < teamSize; i++)
    {
        if (team[i]) team[i]->resetPV();
    }
}

int        Player::getTeamSize() const { return teamSize; }
int        Player::getTeamMax()  const { return teamMax;  }
Character* Player::getTeamCharacter(int index) const
{
    assert(index >= 0 && index < teamSize);
    return team[index];
}

// Fragments

void Player::addFragments(const string& charName, int amount)
{
    for (int i = 0; i < nbFrag; i++)
    {
        if (fragments[i].charName == charName)
        {
            fragments[i].count += amount;
            if (fragments[i].count < 0) fragments[i].count = 0;
            // Auto-débloquer si suffisamment de fragments et perso dans la bank
            if (fragments[i].count >= UNLOCK_FRAGMENT_COST && !isUnlocked(charName))
            {
                for (int j = 0; j < nbBank; j++)
                {
                    if (bank[j] && bank[j]->getName() == charName)
                    {
                        addToUnlocked(bank[j]);
                        break;
                    }
                }
            }
            return;
        }
    }
    if (amount <= 0) return;
    // Nouvelle entrée
    if (nbFrag >= maxFrag)
    {
        int        newMax  = maxFrag * 2;
        FragEntry* newTab  = new FragEntry[newMax];
        for (int i = 0; i < nbFrag; i++) newTab[i] = fragments[i];
        delete[] fragments;
        fragments = newTab;
        maxFrag   = newMax;
    }
    fragments[nbFrag].charName = charName;
    fragments[nbFrag].count    = amount;
    nbFrag++;
}

int Player::getFragments(const string& charName) const
{
    for (int i = 0; i < nbFrag; i++)
    {
        if (fragments[i].charName == charName)
            return fragments[i].count;
    }
    return 0;
}

int              Player::getNbFrag()         const { return nbFrag;            }
const FragEntry& Player::getFragEntry(int i) const { return fragments[i];      }
int              Player::getXPThreshold()    const { return accountLvl * accountLvl * 600; }

// Progression

string Player::getPseudo()     const { return pseudo;     }
int    Player::getLvl()        const { return lvl;        }
int    Player::getAccountLvl() const { return accountLvl; }
int    Player::getXP()         const { return xp;         }
int    Player::getBerries()    const { return berries;     }

void Player::setLvl(int l)        { assert(l >= 1); lvl = l;        }
void Player::addLvl(int l)        { assert(l > 0);  lvl += l;       }
void Player::setAccountLvl(int l) { assert(l >= 1); accountLvl = l; }
void Player::setXP(int x)         { assert(x >= 0); xp = x;         }
void Player::setBerries(int b)    { assert(b >= 0); berries = b;     }
void Player::addBerries(int b)    { berries += b; if (berries < 0) berries = 0; }

// XP → niveau de compte (seuil progressif : lvl² * 600)
// L1→2: 600, L2→3: 2400, L3→4: 5400, L5→6: 15000, L9→10: 48600
void Player::addXP(int amount)
{
    assert(amount >= 0);
    xp += amount;
    int threshold = accountLvl * accountLvl * 600;
    while (xp >= threshold && accountLvl < 10)
    {
        xp        -= threshold;
        accountLvl++;
        if (teamMax < 10) addTeamSize(1);
        threshold = accountLvl * accountLvl * 600;
    }
}
