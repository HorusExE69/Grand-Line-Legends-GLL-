/**
 * @file Family.cpp
 * @brief Implémentation de Family et définition des familles One Piece.
 */

#include "Family.h"
#include <cassert>

using namespace std;

Family::Family(const string& familyName, float atk, float pv, float def)
{
    name      = familyName;
    nbMembers = 0;
    bonusAtk  = atk;
    bonusPV   = pv;
    bonusDef  = def;
}

void Family::addMember(const string& charName)
{
    assert(nbMembers < FAMILY_MAX_MEMBERS);
    members[nbMembers++] = charName;
}

bool Family::hasMember(const string& charName) const
{
    for (int i = 0; i < nbMembers; i++)
    {
        if (members[i] == charName)
            return true;
    }
    return false;
}

int Family::countPresent(const string* teamNames, int teamSize) const
{
    assert(teamNames != nullptr || teamSize == 0);
    int count = 0;
    for (int i = 0; i < teamSize; i++)
    {
        if (hasMember(teamNames[i]))
            count++;
    }
    return count;
}

string Family::getName()      const { return name;      }
float       Family::getBonusAtk()  const { return bonusAtk;  }
float       Family::getBonusPV()   const { return bonusPV;   }
float       Family::getBonusDef()  const { return bonusDef;  }
int         Family::getNbMembers() const { return nbMembers;  }

// Définition des familles

Family** createFamilies()
{
    Family** fams = new Family*[NB_FAMILIES];

    // Les Monkey D (+25% PV si 3 membres)
    fams[0] = new Family("Les Monkey D", 0.0f, 0.25f, 0.0f);
    fams[0]->addMember("Garp");
    fams[0]->addMember("Luffy");
    fams[0]->addMember("Dragon");

    // Le Trio de Frères (+25% Attaque si 3 membres)
    fams[1] = new Family("Trio de Freres", 0.25f, 0.0f, 0.0f);
    fams[1]->addMember("Sabo");
    fams[1]->addMember("Ace");
    fams[1]->addMember("Luffy");

    // Les Révolutionnaires (+15% Def si 3 membres)
    fams[2] = new Family("Les Revolutionnaires", 0.0f, 0.0f, 0.15f);
    fams[2]->addMember("Sabo");
    fams[2]->addMember("Dragon");
    fams[2]->addMember("Ivankov");

    // Les Empereurs (+20% PV si 2+ membres)
    fams[3] = new Family("Les Empereurs", 0.0f, 0.20f, 0.0f);
    fams[3]->addMember("BarbeBlanche");
    fams[3]->addMember("BarbeNoire");
    fams[3]->addMember("Shanks");
    fams[3]->addMember("Kaido");
    fams[3]->addMember("BigMom");

    // Les Amiraux (+20% Def si 2+ membres)
    fams[4] = new Family("Les Amiraux", 0.0f, 0.0f, 0.20f);
    fams[4]->addMember("Akainu");
    fams[4]->addMember("Aokiji");
    fams[4]->addMember("Kizaru");
    fams[4]->addMember("Fujitora");

    // Les Mugiwara (+20% Atk si 4+ membres)
    fams[5] = new Family("Les Mugiwara", 0.20f, 0.0f, 0.0f);
    fams[5]->addMember("Luffy");
    fams[5]->addMember("Zoro");
    fams[5]->addMember("Nami");
    fams[5]->addMember("Usopp");
    fams[5]->addMember("Sanji");
    fams[5]->addMember("Chopper");
    fams[5]->addMember("Robin");
    fams[5]->addMember("Franky");
    fams[5]->addMember("Brook");

    // La Marine (+15% Def si 3+ membres)
    fams[6] = new Family("La Marine", 0.0f, 0.0f, 0.15f);
    fams[6]->addMember("Garp");
    fams[6]->addMember("Akainu");
    fams[6]->addMember("Aokiji");
    fams[6]->addMember("Kizaru");
    fams[6]->addMember("Smoker");
    fams[6]->addMember("Sengoku");

    // L'Équipage de Barbe Blanche (+30% Atk si 3+ membres)
    fams[7] = new Family("Equipage Barbe Blanche", 0.30f, 0.0f, 0.0f);
    fams[7]->addMember("BarbeBlanche");
    fams[7]->addMember("Marco");
    fams[7]->addMember("Ace");

    return fams;
}
