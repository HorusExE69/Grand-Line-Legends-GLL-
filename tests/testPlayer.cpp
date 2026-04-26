/**
 * @file testPlayer.cpp
 * @brief Tests de régression pour la classe Player.
 *
 * Compile avec : make test_player
 * Exécute avec : ./bin/testPlayer
 */

#include "../src/model/Player.h"
#include "../src/model/Characters.h"
#include <cassert>
#include <cstdio>

// ─── Tests ───────────────────────────────────────────────────────────────────

static void testConstructeur()
{
    Player p("TestJoueur");
    assert(p.getPseudo()     == "TestJoueur");
    assert(p.getBerries()    == 500);
    assert(p.getAccountLvl()== 1);
    assert(p.getXP()         == 0);
    assert(p.getNbBank()     == 0);
    assert(p.getNbUnlock()   == 0);
    assert(p.getTeamSize()   == 0);
    assert(p.getTeamMax()    == 1);
    printf("[OK] Player : constructeur\n");
}

static void testBankAddGet()
{
    Player     p("Test");
    Character* c = new Character();
    p.addToBank(c);
    assert(p.getNbBank() == 1);
    assert(p.getBankCharacter(0) == c);
    printf("[OK] Player : addToBank / getBankCharacter\n");
}

static void testUnlock()
{
    Player     p("Test");
    Character* c = new Character();
    p.addToBank(c);

    // Pas encore débloqué
    assert(!p.isUnlocked(""));

    // Débloqué via addToUnlocked
    p.addToUnlocked(c);
    assert(p.getNbUnlock() == 1);
    assert(p.getUnlockCharacter(0) == c);
    printf("[OK] Player : addToUnlocked / isUnlocked\n");
}

static void testTeam()
{
    Player     p("Test");
    Character* c = new Character();
    p.addToBank(c);
    p.addToUnlocked(c);

    assert(p.getTeamMax() == 1);
    bool added = p.addToTeam(c);
    assert(added);
    assert(p.getTeamSize() == 1);

    // Deuxième ajout du même perso refusé
    bool again = p.addToTeam(c);
    assert(!again);

    p.removeFromTeam(0);
    assert(p.getTeamSize() == 0);
    printf("[OK] Player : addToTeam / removeFromTeam\n");
}

static void testAddTeamSize()
{
    Player p("Test");
    assert(p.getTeamMax() == 1);
    p.addTeamSize(4);
    assert(p.getTeamMax() == 5);
    printf("[OK] Player : addTeamSize\n");
}

static void testBerries()
{
    Player p("Test");
    p.setBerries(1000);
    assert(p.getBerries() == 1000);
    p.addBerries(-300);
    assert(p.getBerries() == 700);
    p.addBerries(-10000); // ne doit pas passer en négatif
    assert(p.getBerries() == 0);
    printf("[OK] Player : berries\n");
}

static void testFragments()
{
    Player p("Test");
    p.addFragments("Luffy", 50);
    assert(p.getFragments("Luffy") == 50);
    p.addFragments("Luffy", 60);
    assert(p.getFragments("Luffy") == 110);
    assert(p.getFragments("Zoro")  == 0);
    printf("[OK] Player : fragments\n");
}

static void testXP()
{
    Player p("Test");
    assert(p.getAccountLvl() == 1);
    assert(p.getTeamMax()    == 1);

    p.addXP(1000); // Seuil niveau 1 = 1*1000
    assert(p.getAccountLvl() == 2);
    assert(p.getTeamMax()    == 2); // un slot débloqué
    printf("[OK] Player : addXP / montee de niveau\n");
}

static void testChangePseudo()
{
    Player p("Ancien");
    p.changePseudo("Nouveau");
    assert(p.getPseudo() == "Nouveau");
    printf("[OK] Player : changePseudo\n");
}

static void testUnlockCharacterByFragments()
{
    Player     p("Test");
    Character* c = new Character();
    p.addToBank(c);

    // Pas encore assez de fragments
    bool unlocked = p.unlockCharacter(c);
    assert(!unlocked);
    assert(!p.isUnlocked(c->getName()));

    // Donner exactement le seuil de déblocage
    p.addFragments(c->getName(), UNLOCK_FRAGMENT_COST);
    assert(p.getFragments(c->getName()) >= UNLOCK_FRAGMENT_COST);

    // Le débloquer : doit réussir et déduire les fragments
    unlocked = p.unlockCharacter(c);
    assert(unlocked);
    assert(p.isUnlocked(c->getName()));
    assert(p.getFragments(c->getName()) == 0);

    printf("[OK] Player : unlockCharacter via fragments\n");
}

static void testAddXPWithLevelUp()
{
    Player p("Test");
    assert(p.getAccountLvl() == 1);
    assert(p.getXPThreshold() == 600); // 1*1*600

    // Monter au niveau 2
    p.addXP(600);
    assert(p.getAccountLvl() == 2);
    assert(p.getXP()         == 0);
    assert(p.getTeamMax()    == 2);

    // Seuil niveau 2 → 3 : 2*2*600 = 2400
    assert(p.getXPThreshold() == 2400);

    // Monter au niveau 3 : ajouter exactement le seuil
    p.addXP(2400);
    assert(p.getAccountLvl() == 3);
    assert(p.getTeamMax()    == 3);

    printf("[OK] Player : addXP avec montees de niveau multiples\n");
}

// ─── Main ────────────────────────────────────────────────────────────────────

int main()
{
    printf("=== Tests Player ===\n");
    testConstructeur();
    testBankAddGet();
    testUnlock();
    testTeam();
    testAddTeamSize();
    testBerries();
    testFragments();
    testXP();
    testChangePseudo();
    testUnlockCharacterByFragments();
    testAddXPWithLevelUp();
    printf("=== Tous les tests Player passes ===\n");
    return 0;
}
