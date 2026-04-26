/**
 * @file testCampaign.cpp
 * @brief Tests de régression pour la classe Campaign et SaveData.
 *
 * Compile avec : make test_campaign
 * Exécute avec : ./bin/testCampaign
 */

#include "../src/model/Campaign.h"
#include "../src/model/Player.h"
#include "../src/model/SaveData.h"
#include <cassert>
#include <cstdio>
#include <fstream>

// ─── Tests ───────────────────────────────────────────────────────────────────

static void testNbArcs()
{
    Campaign c;
    assert(c.getNbArcs() == NB_ARCS);
    assert(c.getNbArcs() == 18); // 17 arcs narratifs + arc Pantheon final
    printf("[OK] Campaign : nombre d'arcs = %d\n", NB_ARCS);
}

static void testArcNames()
{
    Campaign c;
    // Vérifier quelques arcs clés
    assert(c.getArc(0)->name  == "Romance Dawn");
    assert(c.getArc(9)->name  == "Alabasta");
    assert(c.getArc(16)->name == "Marine Ford");
    assert(c.getArc(17)->name == "Pantheon");
    printf("[OK] Campaign : noms des arcs\n");
}

static void testArcBoss()
{
    Campaign c;
    assert(c.getArc(0)->bossName  == "Morgan");
    assert(c.getArc(9)->bossName  == "Crocodile");
    assert(c.getArc(15)->bossName == "Magellan");
    printf("[OK] Campaign : noms des boss\n");
}

static void testArcEpisodes()
{
    Campaign c;
    // Chaque arc doit avoir au moins un épisode boss
    for (int i = 0; i < NB_ARCS; i++)
    {
        Arc* arc = c.getArc(i);
        assert(arc->nbEpisodes > 0);
        // Le dernier épisode doit être un boss
        assert(arc->episodes[arc->nbEpisodes - 1]->isBoss);
    }
    printf("[OK] Campaign : structure des episodes\n");
}

static void testNavigation()
{
    Campaign c;
    assert(c.getCurrentArcIdx() == 0);

    c.arcDown();
    assert(c.getCurrentArcIdx() == 1);

    c.arcUp();
    assert(c.getCurrentArcIdx() == 0);

    // Limites
    c.arcUp(); // ne doit pas passer en dessous de 0
    assert(c.getCurrentArcIdx() == 0);

    c.selectArc(NB_ARCS - 1);
    c.arcDown(); // ne doit pas dépasser NB_ARCS-1
    assert(c.getCurrentArcIdx() == NB_ARCS - 1);

    printf("[OK] Campaign : navigation arcs\n");
}

static void testEpisodeNavigation()
{
    Campaign c;
    c.selectArc(0);
    assert(c.getCurrentEpIdx() == 0);

    c.episodeDown();
    assert(c.getCurrentEpIdx() == 1);

    c.episodeUp();
    assert(c.getCurrentEpIdx() == 0);

    printf("[OK] Campaign : navigation episodes\n");
}

static void testBanned()
{
    Campaign c;
    // Arc 8 (Drum Island) : Nami est bannie
    Arc* drumIsland = c.getArc(8);
    assert(drumIsland->isBanned("Nami"));
    assert(!drumIsland->isBanned("Luffy"));
    printf("[OK] Campaign : personnages bannis\n");
}

static void testUnlockChance()
{
    Campaign c;
    // Arc 0 : Zoro 100%
    assert(c.getArc(0)->unlockChance == 1.0f);
    // Arc 1 : Nami 50%
    assert(c.getArc(1)->unlockChance == 0.5f);
    printf("[OK] Campaign : chances de deblocage\n");
}

static void testCompletion()
{
    Campaign c;
    c.selectArc(0);
    c.selectEpisode(0);
    assert(!c.getCurrentEpisode()->completed);

    c.completeCurrentEpisode();
    assert(c.getCurrentEpisode()->completed);
    printf("[OK] Campaign : completion episode\n");
}

static void testSaveDataSaveLoad()
{
    // Préparer un Player et une Campaign avec un état connu
    Player   p("TestSave");
    Campaign c;

    p.setBerries(1234);
    p.setAccountLvl(3);

    Character* ch = new Character();
    p.addToBank(ch);
    p.addToUnlocked(ch);
    p.addFragments(ch->getName(), 42);

    c.selectArc(2);
    c.selectEpisode(1);
    c.completeCurrentEpisode();

    // Sauvegarder dans un fichier temporaire
    const char* tmpPath = "/tmp/gll_test_save.txt";
    SaveData::save(tmpPath, p, c);
    assert(SaveData::exists(tmpPath));

    // Vérifier le résumé
    std::string summary = SaveData::getSummary(tmpPath);
    assert(!summary.empty());

    // Charger dans un nouveau Player + Campaign vierges
    Player   p2("Vide");
    Campaign c2;

    bool ok = SaveData::load(tmpPath, p2, c2);
    assert(ok);
    assert(p2.getPseudo()     == "TestSave");
    assert(p2.getBerries()    == 1234);
    assert(p2.getAccountLvl() == 3);

    printf("[OK] SaveData : save / load\n");
}

// ─── Main ────────────────────────────────────────────────────────────────────

int main()
{
    printf("=== Tests Campaign ===\n");
    testNbArcs();
    testArcNames();
    testArcBoss();
    testArcEpisodes();
    testNavigation();
    testEpisodeNavigation();
    testBanned();
    testUnlockChance();
    testCompletion();
    testSaveDataSaveLoad();
    printf("=== Tous les tests Campaign passes ===\n");
    return 0;
}
