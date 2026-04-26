/**
 * @file Battle.cpp
 * @brief Implémentation de Battle.
 */

#include "Battle.h"
#include <cassert>
#include <cstdlib>

using namespace std;

Battle::Battle(Player* p)
{
    player  = p;
    enemy   = new Player("Ennemi");
    turn    = 0;
    nbLog   = 0;
    map     = new BattleMap;
}

Battle::~Battle()
{
    delete enemy;
    delete map;
}

// Initialise la grille en plaçant les équipes
void Battle::init()
{
    assert(player != nullptr);
    map->init(player, enemy);
}

// Déroulement du combat

void Battle::playAllTurns()
{
    while (!isOver())
    {
        playTurn();
        turn++;
    }
}

bool Battle::playNextTurn()
{
    if (isOver()) return false;
    playTurn();
    turn++;
    return true;
}

void Battle::playTurn()
{
    clearLog();

    // Appliquer les effets passifs avant les attaques
    for (int i = 0; i < player->getTeamSize(); i++)
    {
        Character* c = player->getTeamCharacter(i);
        if (c && c->getPV() > 0) c->applyPassives();
    }
    for (int i = 0; i < enemy->getTeamSize(); i++)
    {
        Character* c = enemy->getTeamCharacter(i);
        if (c && c->getPV() > 0) c->applyPassives();
    }

    int      orderSize = 0;
    Square** order     = getOrder(orderSize);

    for (int i = 0; i < orderSize; i++)
    {
        bool side = (order[i]->x == 0); // true = joueur, false = ennemi
        playCharacter(order[i], side);
    }

    delete[] order;
}

void Battle::playCharacter(Square* sq, bool playerSide)
{
    if (!sq || !sq->inmate || sq->inmate->getPV() <= 0) return;
    if (sq->inmate->stunned) { sq->inmate->stunned = false; return; }

    Character* attacker = sq->inmate;
    Capacity*  capa     = attacker->chooseCapa();
    if (!capa) return;

    capa->launcher = sq;

    bool isDmgCapa = (capa->getEffect() &&
                      capa->getEffect()->getType() == EffectType::Damage);

    // Déterminer le côté cible selon le type d'effet
    TargetType tgtType = capa->getEffect() ? capa->getEffect()->getTarget() : TargetType::Enemy;
    bool targetFriendly = (tgtType == TargetType::Self ||
                           tgtType == TargetType::Ally ||
                           tgtType == TargetType::AllAllies);

    Player* ownTeam   = playerSide ? player : enemy;
    Player* foeTeam   = playerSide ? enemy  : player;
    int     ownSide   = playerSide ? 0 : 1;
    int     foeSide   = playerSide ? 1 : 0;

    Player* targets    = targetFriendly ? ownTeam : foeTeam;
    int     targetSide = targetFriendly ? ownSide  : foeSide;

    // Self : cibler l'attaquant lui-même
    if (tgtType == TargetType::Self)
    {
        capa->addTarget(sq);
        capa->use();
        addLog(attacker->getName(), capa->getName(), attacker->getName(), 0);
        return;
    }

    int size = targets->getTeamSize();
    if (size == 0) return;

    // AllAllies / AllEnemies / All : cibler tous les vivants du côté
    if (tgtType == TargetType::AllAllies || tgtType == TargetType::AllEnemies ||
        tgtType == TargetType::All)
    {
        for (int i = 0; i < size; i++)
        {
            Character* t = targets->getTeamCharacter(i);
            if (!t || t->getPV() <= 0) continue;
            int effectiveDmg = 0;
            if (isDmgCapa)
            {
                // Dodge check
                if (t->getHakiO() > 0 && (rand() % 500) < t->getHakiO()) continue;
                float mult = 1.0f + attacker->getHakiA() / 500.0f;
                int   rDiff = attacker->getHakiR() - t->getHakiR();
                float rBonus = rDiff / 800.0f;
                if (rBonus >  0.20f) rBonus =  0.20f;
                if (rBonus < -0.20f) rBonus = -0.20f;
                mult += rBonus;
                if (mult < 0.60f) mult = 0.60f;
                if (mult > 2.00f) mult = 2.00f;
                effectiveDmg = (int)(capa->getDamage() * mult);
                int savedVal = capa->getEffect()->getValue();
                capa->getEffect()->setValue(effectiveDmg);
                capa->addTarget(&map->getSquare(targetSide, i));
                capa->use();
                capa->getEffect()->setValue(savedVal);
            }
            else
            {
                capa->addTarget(&map->getSquare(targetSide, i));
                capa->use();
            }
            addLog(attacker->getName(), capa->getName(), t->getName(), effectiveDmg);
        }
        return;
    }

    // Cible unique : chercher un vivant aléatoire
    int tries = 0;
    int idx   = rand() % size;
    Character* defender = targets->getTeamCharacter(idx);
    while ((!defender || defender->getPV() <= 0) && tries < size)
    {
        idx = (idx + 1) % size;
        defender = targets->getTeamCharacter(idx);
        tries++;
    }
    if (!defender || defender->getPV() <= 0) return;

    // Haki de l'Observation : chance d'esquiver (dégâts seulement)
    if (isDmgCapa && defender->getHakiO() > 0)
    {
        if ((rand() % 500) < defender->getHakiO())
        {
            addLog(attacker->getName(), capa->getName(),
                   defender->getName() + " [ESQUIVE]", 0);
            return;
        }
    }

    int effectiveDmg = capa->getDamage();

    if (isDmgCapa)
    {
        float mult = 1.0f + attacker->getHakiA() / 500.0f;
        int   rDiff  = attacker->getHakiR() - defender->getHakiR();
        float rBonus = rDiff / 800.0f;
        if (rBonus >  0.20f) rBonus =  0.20f;
        if (rBonus < -0.20f) rBonus = -0.20f;
        mult += rBonus;
        if (mult < 0.60f) mult = 0.60f;
        if (mult > 2.00f) mult = 2.00f;
        effectiveDmg = (int)(capa->getDamage() * mult);

        int savedVal = capa->getEffect()->getValue();
        capa->getEffect()->setValue(effectiveDmg);
        capa->addTarget(&map->getSquare(targetSide, idx));
        capa->use();
        capa->getEffect()->setValue(savedVal);
    }
    else
    {
        capa->addTarget(&map->getSquare(targetSide, idx));
        capa->use();
    }

    addLog(attacker->getName(), capa->getName(), defender->getName(), effectiveDmg);
}

// État du combat

bool Battle::isDead(Player* p) const
{
    assert(p != nullptr);
    for (int i = 0; i < p->getTeamSize(); i++)
    {
        Character* c = p->getTeamCharacter(i);
        if (c && c->getPV() > 0) return false;
    }
    return true;
}

bool Battle::isOver() const
{
    return isDead(player) || isDead(enemy);
}

Player* Battle::getWinner() const
{
    if (isDead(player) && isDead(enemy)) return nullptr;
    if (isDead(player))                  return enemy;
    if (isDead(enemy))                   return player;
    return nullptr;
}

// Ordre d'attaque (tri bulle par vitesse croissante)

Square** Battle::getOrder(int& outSize) const
{
    int maxN = player->getTeamSize() + enemy->getTeamSize();
    Square** order = new Square*[maxN];
    int      k     = 0;

    for (int i = 0; i < player->getTeamSize(); i++)
    {
        Character* ch = player->getTeamCharacter(i);
        if (ch && ch->getPV() > 0)
            order[k++] = &map->getSquare(0, i);
    }
    for (int j = 0; j < enemy->getTeamSize(); j++)
    {
        Character* ch = enemy->getTeamCharacter(j);
        if (ch && ch->getPV() > 0)
            order[k++] = &map->getSquare(1, j);
    }

    // Tri bulle par vitesse décroissante (le plus rapide joue en premier)
    for (int i = 0; i < k - 1; i++)
    {
        for (int j = 0; j < k - i - 1; j++)
        {
            if (order[j] && order[j + 1] &&
                order[j]->inmate && order[j + 1]->inmate &&
                order[j]->inmate->speed < order[j + 1]->inmate->speed)
            {
                Square* tmp  = order[j];
                order[j]     = order[j + 1];
                order[j + 1] = tmp;
            }
        }
    }

    outSize = k;
    return order;
}

// Log

void Battle::addLog(const string& atk, const string& cap,
                    const string& tgt, int val)
{
    if (nbLog >= MAX_LOG) return;
    log[nbLog].attacker = atk;
    log[nbLog].capa     = cap;
    log[nbLog].target   = tgt;
    log[nbLog].value    = val;
    nbLog++;
}

void             Battle::clearLog()          { nbLog = 0; }
int              Battle::getNbLog()    const { return nbLog; }
const BattleLog& Battle::getLog(int i) const
{
    assert(i >= 0 && i < nbLog);
    return log[i];
}

// Accesseurs

Player* Battle::getPlayer() const { return player; }
Player* Battle::getEnemy()  const { return enemy;  }
int     Battle::getTurn()   const { return turn;   }
