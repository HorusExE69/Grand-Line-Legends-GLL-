#include "Capacity.h"
#include <iostream>

using namespace std;

Capacity::Capacity(const string& name, int dmg, int hl, int lt)
{
    nameCapa = name;
    damage = dmg;
    heal = hl;
    loadTime = lt;

    typeC.t1 = TypeC::other;
    typeC.t2 = TypeC::other;

    launcher = nullptr;
    effects = nullptr;
    nbEffects = 0;

    targets = nullptr;
    nbTargets = 0;
}

Capacity::~Capacity()
{
    if (effects != nullptr)
    {
        for (int i = 0; i < nbEffects; ++i)
            delete effects[i];
        delete[] effects;
    }

    if (targets != nullptr)
        delete[] targets;
}

// Ajouter un effet
void Capacity::addEffect(Effect* e)
{
    Effect** tmp = new Effect*[nbEffects + 1];
    for (int i = 0; i < nbEffects; ++i)
        tmp[i] = effects[i];
    tmp[nbEffects] = e;
    delete[] effects;
    effects = tmp;
    nbEffects++;
}

// Ajouter une cible
void Capacity::addTarget(Square* s)
{
    Square** tmp = new Square*[nbTargets + 1];
    for (int i = 0; i < nbTargets; ++i)
        tmp[i] = targets[i];
    tmp[nbTargets] = s;
    delete[] targets;
    targets = tmp;
    nbTargets++;
}

// Appliquer la capacité
void Capacity::use()
{
    if (!launcher)
    {
        cout << "Erreur : capacité sans lanceur" << endl;
        return;
    }

    for (int i = 0; i < nbEffects; ++i)
    {
        for (int j = 0; j < nbTargets; ++j)
        {
            if (targets[j] && targets[j]->inmate)
                targets[j]->inmate->applyEffect(effects[i]);
        }
    }
}

// Getters
const string& Capacity::getName() const { return nameCapa; }
int Capacity::getDamage() const { return damage; }
int Capacity::getHeal() const { return heal; }