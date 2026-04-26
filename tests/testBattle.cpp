/**
 * @file testBattle.cpp
 * @brief Tests de régression pour la classe Battle.
 *
 * Compile avec : make test_battle
 * Exécute avec : ./bin/testBattle
 */

#include "../src/controller/Battle.h"
#include "../src/model/Player.h"
#include "../src/model/Characters.h"
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <ctime>

// ─── Helpers ─────────────────────────────────────────────────────────────────

// Crée un personnage minimal avec des PV
static Character* makeChar(int pv, int speed)
{
    Character* c = new Character();
    // Les PV et la vitesse ne sont pas settables depuis l'extérieur via setters
    // On utilise updateLvl pour simuler un personnage non nul
    // (Dans un vrai test d'intégration on lirait le CSV)
    (void)pv; (void)speed;
    return c;
}

// ─── Tests ───────────────────────────────────────────────────────────────────

static void testBattleConstruction()
{
    Player p("Joueur");
    Battle b(&p);
    assert(b.getPlayer()  == &p);
    assert(b.getEnemy()   != nullptr);
    assert(b.getTurn()    == 0);
    assert(b.isOver()     == true); // équipes vides → tout mort
    printf("[OK] Battle : construction\n");
}

static void testBattleOrder()
{
    // Tester getOrder sans personnages (retourne tableau vide)
    Player p("Joueur");
    Battle b(&p);
    int    size = 0;
    Square** order = b.getOrder(size);
    assert(size == 0);
    delete[] order;
    printf("[OK] Battle : getOrder (equipes vides)\n");
}

static void testIsDead()
{
    Player p("Test");
    Battle b(&p);
    // Équipe vide → isDead retourne true
    assert(b.isDead(&p) == true);
    printf("[OK] Battle : isDead (equipe vide)\n");
}

static void testLog()
{
    Player p("Joueur");
    Battle b(&p);
    assert(b.getNbLog() == 0);
    b.clearLog();
    assert(b.getNbLog() == 0);
    printf("[OK] Battle : log\n");
}

static void testWinnerWhenBothDead()
{
    Player p("Joueur");
    Battle b(&p);
    // Les deux équipes sont vides → getWinner retourne nullptr
    assert(b.getWinner() == nullptr);
    printf("[OK] Battle : getWinner (egalite)\n");
}

// ─── Main ────────────────────────────────────────────────────────────────────

int main()
{
    srand(static_cast<unsigned int>(time(nullptr)));
    printf("=== Tests Battle ===\n");
    testBattleConstruction();
    testBattleOrder();
    testIsDead();
    testLog();
    testWinnerWhenBothDead();
    printf("=== Tous les tests Battle passes ===\n");
    return 0;
}
