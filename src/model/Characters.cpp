/**
 * @file Characters.cpp
 * @brief Implémentation de Character.
 */

#include "Characters.h"
#include "Capacity.h"
#include "Effects.h"
#include "Utils.h"
#include <fstream>
#include <cassert>
#include <cstdlib>

using namespace std;

// Constructeurs / Destructeur

Character::Character(void)
{
    name      = "";
    type      = Type_::atk_;
    rarity    = Rarity::c;
    pv        = 0;
    pvBase    = 0;
    speed     = 0;
    lvl       = 1;
    tabCapa   = nullptr;
    nbCapa    = 0;
    maxCapa   = 0;
    tabEffects = nullptr;
    nbEffects  = 0;
    maxEffects = 3;
    pos        = nullptr;
    hakiA      = 0;
    hakiO      = 0;
    hakiR      = 0;
    resistValue   = 0;
    bleedingValue = 0;
    isBleeding    = false;
    stunned       = false;
}

// Constructeur depuis une ligne CSV : id,name,type,rarity,pv,speed,hakiR,hakiA,hakiO
Character::Character(const string& line, const string& path)
{
    nbCapa   = 0;
    maxCapa  = 10;
    tabCapa  = new Capacity*[maxCapa];

    tabEffects = nullptr;
    nbEffects  = 0;
    maxEffects = 3;
    pos        = nullptr;
    resistValue   = 0;
    bleedingValue = 0;
    isBleeding    = false;
    stunned       = false;

    string l   = line;
    size_t cur = 0;

    // Ignorer l'id
    cur = l.find(',');
    l   = l.substr(cur + 1);

    // name
    cur  = l.find(',');
    name = l.substr(0, cur);
    l    = l.substr(cur + 1);

    // type
    cur  = l.find(',');
    type = stringToType_(l.substr(0, cur));
    l    = l.substr(cur + 1);

    // rarity
    cur    = l.find(',');
    rarity = stringToRarity(l.substr(0, cur));
    l      = l.substr(cur + 1);

    // pv
    cur  = l.find(',');
    pv   = stoi(clean(l.substr(0, cur)));
    pvBase = pv;
    l    = l.substr(cur + 1);

    // speed
    cur   = l.find(',');
    speed = stoi(clean(l.substr(0, cur)));
    l     = l.substr(cur + 1);

    // hakiR
    cur   = l.find(',');
    hakiR = stoi(clean(l.substr(0, cur)));
    l     = l.substr(cur + 1);

    // hakiA
    cur   = l.find(',');
    hakiA = stoi(clean(l.substr(0, cur)));
    l     = l.substr(cur + 1);

    // hakiO
    hakiO = (!l.empty()) ? stoi(clean(l)) : 0;

    lvl = 1;
    loadCapacities(path);
}

Character::~Character(void)
{
    for (int i = 0; i < nbCapa; i++)
        delete tabCapa[i];
    delete[] tabCapa;

    if (tabEffects != nullptr)
        delete[] tabEffects;

    pos = nullptr; // la case n'appartient pas au personnage
}

// Chargement des capacités

void Character::loadCapacities(const string& pathCapaFolder)
{
    string   path = pathCapaFolder + name + ".csv";
    ifstream file(path);
    if (!file.is_open()) return;

    string line;
    getline(file, line); // ignorer l'en-tête

    while (getline(file, line))
    {
        if (line.empty()) continue;
        Capacity* c = new Capacity(line);
        addToCapa(c);
    }
}

// Combat

void Character::applyEffect(Effect* e)
{
    assert(e != nullptr);

    bool canStore = nbEffects < maxEffects;
    if (canStore)
    {
        if (tabEffects == nullptr)
            tabEffects = new Effect*[maxEffects];
        tabEffects[nbEffects++] = e;
    }

    switch (e->getType())
    {
        case EffectType::Damage:
        {
            // La résistance réduit les dégâts reçus (pourcentage 0-100)
            int dmg = e->getValue();
            if (resistValue > 0) {
                dmg = dmg * (100 - resistValue) / 100;
                if (dmg < 0) dmg = 0;
            }
            pv -= dmg;
            if (pv < 0) pv = 0;
            break;
        }

        case EffectType::Heal:
            pv += e->getValue();
            if (pvBase > 0 && pv > pvBase) pv = pvBase; // plafond aux PV de base
            break;

        case EffectType::Buff:
            // Buff : augmente le haki d'armement (attaque) de la valeur
            if (e->getValue() > 0) hakiA += e->getValue();
            break;

        case EffectType::Debuff:
            // Debuff : ralentit la cible
            if (e->getValue() > 0) {
                speed -= e->getValue();
                if (speed < 5) speed = 5;
            }
            break;

        case EffectType::Resist:
            resistValue += e->getValue();
            if (resistValue > 80) resistValue = 80; // résistance max 80%
            break;

        case EffectType::Stun:
            stunned = true;
            break;

        case EffectType::Bleeding:
            bleedingValue = e->getValue();
            isBleeding    = true;
            break;

        case EffectType::Push:
        case EffectType::Pull:
            if (e->getExtraTarget1())
            {
                Square* s1 = pos;
                Square* s2 = e->getExtraTarget1()->pos;
                if (s1 && s2) s1->swap(s2);
            }
            break;

        case EffectType::Swap:
            if (e->getExtraTarget1() && e->getExtraTarget2())
            {
                Square* s1 = e->getExtraTarget1()->pos;
                Square* s2 = e->getExtraTarget2()->pos;
                if (s1 && s2) s1->swap(s2);
            }
            break;

        case EffectType::Other:
            break;
    }
}

void Character::applyPassives()
{
    if (nbCapa == 0 || tabCapa == nullptr) return;

    for (int i = 0; i < nbCapa; i++)
    {
        Capacity* c = tabCapa[i];
        // N'appliquer QUE les capacités passives (percentage >= 100)
        if (!c || !c->getIsPassive()) continue;
        c->use();
    }
}

void Character::addToCapa(Capacity* c)
{
    if (c == nullptr) return;

    if (nbCapa >= maxCapa)
    {
        int       newMax    = (maxCapa == 0) ? 4 : maxCapa * 2;
        Capacity** newTab   = new Capacity*[newMax];
        for (int i = 0; i < nbCapa; i++) newTab[i] = tabCapa[i];
        delete[] tabCapa;
        tabCapa  = newTab;
        maxCapa  = newMax;
    }
    tabCapa[nbCapa++] = c;
}

// Choisit une capacité active selon les pourcentages (roue de fortune)
Capacity* Character::chooseCapa()
{
    if (nbCapa == 0 || tabCapa == nullptr) return nullptr;

    int total = 0;
    for (int i = 0; i < nbCapa; i++)
    {
        if (!tabCapa[i] || tabCapa[i]->getIsPassive()) continue;
        total += tabCapa[i]->getPercentage();
    }
    if (total == 0) return nullptr;

    int roll = rand() % total;
    int sum  = 0;
    for (int i = 0; i < nbCapa; i++)
    {
        if (!tabCapa[i] || tabCapa[i]->getIsPassive()) continue;
        sum += tabCapa[i]->getPercentage();
        if (roll < sum) return tabCapa[i];
    }
    return nullptr;
}

// Stats

void Character::updateLvl(int nbLvl)
{
    assert(nbLvl > 0);
    lvl    += nbLvl;
    pvBase += nbLvl * 50;
    pv      = pvBase;
    speed  += nbLvl * 2;
    if (speed > 150) speed = 150;
}

void Character::resetPV()
{
    pv = pvBase;
}

void Character::resetCombatState()
{
    // Libérer le tableau d'effets (les pointeurs pourraient être dangling
    // si les capacités ennemies ont été supprimées depuis le dernier combat)
    if (tabEffects != nullptr)
    {
        delete[] tabEffects;
        tabEffects = nullptr;
    }
    nbEffects = 0;

    // Réinitialiser les états temporaires de combat
    isBleeding    = false;
    bleedingValue = 0;
    stunned       = false;
    resistValue   = 0;
}

int Character::upgradeCost() const
{
    // Coût croissant selon la rareté et le niveau
    int base = 500;
    switch (rarity)
    {
        case Rarity::c:  base =  500; break;
        case Rarity::r:  base = 1000; break;
        case Rarity::sr: base = 2000; break;
        case Rarity::e:  base = 5000; break;
        case Rarity::l:  base = 10000; break;
        case Rarity::m:  base = 25000; break;
    }
    return base * lvl;
}

// Accesseurs

Type_  Character::typeC(void) { return type;   }
Rarity Character::rarC(void)  { return rarity; }

bool Character::isSup(void) { return type == Type_::sup_; }
bool Character::isDef(void) { return type == Type_::def_; }
bool Character::isInt(void) { return type == Type_::int_; }
bool Character::isAtk(void) { return type == Type_::atk_; }
bool Character::isSnp(void) { return type == Type_::snp_; }
bool Character::isMag(void) { return type == Type_::mag_; }

bool Character::isC(void)  { return rarity == Rarity::c; }
bool Character::isR(void)  { return rarity == Rarity::r; }
bool Character::isSR(void) { return rarity == Rarity::sr; }
bool Character::isE(void)  { return rarity == Rarity::e; }
bool Character::isL(void)  { return rarity == Rarity::l; }
bool Character::isM(void)  { return rarity == Rarity::m; }

string Character::getName(void)  { return name;    }
int    Character::getPV(void)    { return pv;      }
int    Character::getPVBase(void) const { return pvBase; }
int    Character::getLvl(void)    const { return lvl;    }
int    Character::getSpeed(void)  const { return speed;  }
int    Character::getHakiR(void)  const { return hakiR;  }
int    Character::getHakiO(void)  const { return hakiO;  }
int    Character::getHakiA(void)  const { return hakiA;       }
int    Character::getResistValue()   const { return resistValue;  }
int    Character::getBleedingValue() const { return bleedingValue;}
bool   Character::getIsBleeding()    const { return isBleeding;   }
bool   Character::isStunned()        const { return stunned;      }
void   Character::setStunned(bool val)     { stunned = val;       }
int    Character::getNbCapa()     const { return nbCapa;  }
Capacity** Character::getTabCapa() const { return tabCapa; }
