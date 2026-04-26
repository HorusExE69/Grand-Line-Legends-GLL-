/**
 * @file Game.cpp
 * @brief Implémentation du contrôleur principal Game.
 */

#include "Game.h"
#include "../model/Player.h"
#include "../model/Coffre.h"
#include "../view/ViewText.h"
#include "Battle.h"
#include "Event.h"
#include <fstream>
#include <ctime>
#include <cstdlib>
#include <cassert>
#include <string>

using namespace std;

// Constructeur / Destructeur

Game::Game(void)
{
    // Initialiser le générateur aléatoire
    srand(static_cast<unsigned int>(time(nullptr)));

    // Charger les personnages depuis le CSV
    ifstream file("./data/Characters.csv");
    player = new Player(file, "./data/capacities/", "Joueur");
    file.close();

    campaign      = new Campaign();
    settings      = new Settings();
    settings->load("./data/settings.txt");
    shop          = new Shop();
    currentBattle = nullptr;

    selectedIndex = 0;
    saveSlot      = 1;
    state         = GameState::MAIN_MENU;
    statusMsg     = "";
}

Game::~Game(void)
{
    delete player;
    delete campaign;
    delete settings;
    delete shop;
    if (currentBattle) delete currentBattle;
}

// Initialisation

void Game::init(void)
{
    // Démarrer sur le menu principal
    state = GameState::MAIN_MENU;
}

// Mise à jour

void Game::update(Event* ev)
{
    assert(ev != nullptr);

    switch (ev->type)
    {
        case EventType::SAVE_SELECT:
            state = GameState::SAVE_SELECT;
            selectedIndex = 0;
            break;

        case EventType::MAIN_MENU:
            state = GameState::MAIN_MENU;
            selectedIndex = 0;
            break;

        case EventType::SETTINGS:
            state = GameState::SETTINGS;
            selectedIndex = 0;
            break;

        case EventType::PLAY_MENU:
            state = GameState::PLAY_MENU;
            selectedIndex = 0;
            break;

        case EventType::BANK:
            state = GameState::BANK;
            // Ne pas réinitialiser selectedIndex : conserve la position lors du retour depuis BANK_DETAIL
            break;

        case EventType::BANK_DETAIL:
            state = GameState::BANK_DETAIL;
            break;

        case EventType::BANK_UPGRADE:
        {
            Character* c = player->getBankCharacter(selectedIndex);
            if (c && player->isUnlocked(c->getName()))
            {
                int cost  = c->upgradeCost();
                int frags = player->getFragments(c->getName());
                if (frags < UPGRADE_FRAGMENT_COST)
                    statusMsg = "Fragments insuffisants (100 requis pour ameliorer).";
                else if (player->getBerries() < cost)
                    statusMsg = "Berries insuffisants.";
                else
                {
                    player->addBerries(-cost);
                    // Consommer les fragments
                    player->addFragments(c->getName(), -UPGRADE_FRAGMENT_COST);
                    c->updateLvl(1);
                    statusMsg = "Amelioration reussie !";
                }
            }
            state = GameState::BANK_DETAIL;
            break;
        }

        case EventType::BANK_UNLOCK:
        {
            // Débloquer le personnage si 100 fragments disponibles
            Character* c = player->getBankCharacter(selectedIndex);
            if (c && !player->isUnlocked(c->getName()))
            {
                if (!player->unlockCharacter(c))
                    statusMsg = "Fragments insuffisants (100 requis).";
                else
                    statusMsg = "Personnage debloque !";
            }
            state = GameState::BANK_DETAIL;
            break;
        }

        case EventType::BATTLE_RETURN:
            // Retour à l'écran de l'arc courant SANS reset de l'arc/épisode
            state = GameState::CAMPAIGN_ARC;
            selectedIndex = campaign->getCurrentEpIdx();
            break;

        case EventType::BATTLE_RETRY:
            // Retour à la préparation pour changer l'équipe si besoin
            state = GameState::BATTLE_PREPA;
            break;

        case EventType::BATTLE_CONTINUE:
        {
            // Passer à l'épisode suivant; si arc terminé → retour à la liste
            Arc* arc   = campaign->getCurrentArc();
            int  epIdx = campaign->getCurrentEpIdx();
            if (epIdx < arc->nbEpisodes - 1)
            {
                campaign->episodeDown();
                selectedIndex = campaign->getCurrentEpIdx();
                state = GameState::BATTLE_PREPA;
            }
            else
            {
                // Dernier épisode de l'arc → retour à la sélection d'épisodes
                state = GameState::CAMPAIGN_ARC;
                selectedIndex = epIdx;
            }
            break;
        }

        case EventType::CAMPAIGN:
            state = GameState::CAMPAIGN;
            selectedIndex = campaign->getCurrentArcIdx();
            break;

        case EventType::CAMPAIGN_ARC:
        {
            // Arc 0 toujours accessible ; arc i nécessite arc i-1 complété
            bool accessible = (selectedIndex == 0 ||
                               campaign->getArc(selectedIndex - 1)->completed);
            if (accessible)
            {
                state = GameState::CAMPAIGN_ARC;
                campaign->selectArc(selectedIndex);
                selectedIndex = 0;
            }
            else
            {
                statusMsg = "Arc verrouille : completez l'arc precedent d'abord.";
            }
            break;
        }

        case EventType::CAMPAIGN_EPISODE:
        {
            Arc*  arc  = campaign->getCurrentArc();
            bool  accessible = (selectedIndex == 0 ||
                                arc->episodes[selectedIndex - 1]->completed);
            if (accessible)
            {
                campaign->selectEpisode(selectedIndex);
                state = GameState::BATTLE_PREPA;
                selectedIndex = 0;
            }
            else
            {
                statusMsg = "Episode verrouille : completez l'episode precedent d'abord.";
            }
            break;
        }

        case EventType::PLAY:
            state = GameState::BATTLE_PREPA;
            selectedIndex = 0;
            break;

        case EventType::TEAM:
            state = GameState::TEAM;
            break;

        case EventType::TEAM_CHANGE:
            state = GameState::TEAM_CHANGE;
            selectedIndex = 0;
            break;

        case EventType::BATTLE:
        {
            Arc* arc = campaign->getCurrentArc();
            bool hasBanned = false;
            for (int i = 0; i < player->getTeamSize() && !hasBanned; i++)
            {
                Character* c = player->getTeamCharacter(i);
                if (c && arc && arc->isBanned(c->getName()))
                {
                    statusMsg = c->getName() + " est interdit dans cet arc ! Retirez-le de l'equipe.";
                    hasBanned = true;
                }
            }
            if (!hasBanned)
            {
                startBattle();
                state = GameState::BATTLE;
            }
            break;
        }

        case EventType::SHOP:
            // Recharger le shop (offres du jour)
            delete shop;
            shop  = new Shop();
            state = GameState::SHOP;
            selectedIndex = 0;
            break;

        case EventType::SAVE_NEW:
        {
            // Trouver le premier emplacement vide, sinon slot 1
            int slot = 1;
            for (int i = 1; i <= 3; i++) {
                if (!SaveData::exists(SAVE_PATHS[i-1])) { slot = i; break; }
            }
            saveSlot = slot;
            // Réinitialiser le joueur et la campagne
            delete player;
            {
                ifstream file("./data/Characters.csv");
                player = new Player(file, "./data/capacities/", "Joueur");
            }
            delete campaign;
            campaign = new Campaign();
            // Débloquer Luffy par défaut au démarrage d'une nouvelle partie
            for (int i = 0; i < player->getNbBank(); i++) {
                Character* c = player->getBankCharacter(i);
                if (c && c->getName() == "Luffy") {
                    player->addToUnlocked(c);
                    player->addToTeam(c);
                    break;
                }
            }
            state = GameState::PLAY_MENU;
            selectedIndex = 0;
            break;
        }

        case EventType::SAVE_LOAD_1:
        case EventType::SAVE_LOAD_2:
        case EventType::SAVE_LOAD_3:
        {
            int slot = (ev->type == EventType::SAVE_LOAD_1) ? 1 :
                       (ev->type == EventType::SAVE_LOAD_2) ? 2 : 3;
            if (SaveData::exists(SAVE_PATHS[slot-1])) {
                saveSlot = slot;
                // Repartir d'un état propre (level 1) avant d'appliquer la sauvegarde
                // pour éviter l'empilement des niveaux à chaque rechargement
                delete player;
                {
                    ifstream file("./data/Characters.csv");
                    player = new Player(file, "./data/capacities/", "Joueur");
                }
                delete campaign;
                campaign = new Campaign();
                SaveData::load(SAVE_PATHS[slot-1], *player, *campaign);
                state = GameState::PLAY_MENU;
                selectedIndex = 0;
            }
            break;
        }

        case EventType::SAVE:
            save();
            break;

        case EventType::QUIT:
            if (state != GameState::SAVE_SELECT) save();
            state = GameState::QUIT;
            break;

        case EventType::NONE:
        default:
            break;
    }
}

// Combat

void Game::startBattle()
{
    // Nettoyer le combat précédent
    if (currentBattle) { delete currentBattle; currentBattle = nullptr; }

    // Remettre les personnages joueur à leur PV max et purger l'état de combat
    // (effets résiduels, saignement, etc. venant du combat précédent)
    for (int i = 0; i < player->getNbBank(); i++)
    {
        Character* c = player->getBankCharacter(i);
        if (c) { c->resetPV(); c->resetCombatState(); }
    }

    currentBattle = new Battle(player);

    // Construire l'équipe ennemie selon la difficulté de l'épisode
    Episode* ep = campaign->getCurrentEpisode();
    buildEnemyTeam(currentBattle->getEnemy(), ep ? ep->difficulty : 3);

    currentBattle->init();
}

static Character* makeNpc(const string& npcName, const string& type,
                          int pv, int speed, int hakiR, int hakiA, int hakiO)
{
    string line = "0," + npcName + "," + type + ",c,"
                + to_string(pv)    + "," + to_string(speed) + ","
                + to_string(hakiR) + "," + to_string(hakiA) + ","
                + to_string(hakiO);
    return new Character(line, "data/capacities/");
}

static void addNpc(Player* enemy, const string& name, const string& type,
                   int pv, int speed, int hakiR, int hakiA, int hakiO)
{
    Character* npc = makeNpc(name, type, pv, speed, hakiR, hakiA, hakiO);
    enemy->addToBank(npc);
    enemy->addToTeam(npc);
}

void Game::buildEnemyTeam(Player* enemy, int difficulty)
{
    assert(enemy != nullptr);
    assert(difficulty >= 1);

    Arc*     arc = campaign->getCurrentArc();
    Episode* ep  = campaign->getCurrentEpisode();

    struct FactionNpc { const char* name; const char* type; };
    FactionNpc faction = {"SoldatMarine", "def"};

    // Nom du boss et mini-boss selon l'arc
    const char* bossName  = "BossNPC";
    const char* mbossName = "MiniBossNPC";

    if (arc) {
        const string& a = arc->name;
        int epIdx = campaign->getCurrentEpIdx();
        if      (a == "Romance Dawn")   { faction={"SoldatMarine","def"}; bossName="Morgan";       mbossName="Helmeppo"; }
        else if (a == "Orange Town")    { faction={"PirateBaggy","atk"};  bossName="Baggy";        mbossName="Mohji"; }
        else if (a == "Village Sirop")  { faction={"PirateSimple","atk"}; bossName="Kuro";         mbossName="Jango"; }
        else if (a == "Baratie")        { faction={"PirateSimple","atk"}; bossName="Krieg";        mbossName="Gin"; }
        else if (a == "Arlong Park")    { faction={"PirateSimple","atk"}; bossName="Arlong";       mbossName="Hachi"; }
        else if (a == "Logue Town")     { faction={"SoldatMarine","def"}; bossName="Smoker";       mbossName="Tashigi"; }
        else if (a == "Whiskey Peak")   { faction={"BaroqueAgent","int"}; bossName="Mr5";          mbossName="Mr9"; }
        else if (a == "Little Garden")  { faction={"BaroqueAgent","int"}; bossName="Mr3";          mbossName="Goldenweek"; }
        else if (a == "Drum Island")    { faction={"SoldatMarine","def"}; bossName="Wapol";        mbossName="Chess"; }
        else if (a == "Alabasta")       { faction={"BaroqueAgent","int"}; bossName="Crocodile";    mbossName="Mr1"; }
        else if (a == "Skypea")         { faction={"SoldatMarine","def"}; bossName="Ener";         mbossName="Ohm"; }
        else if (a == "Water Seven")    { faction={"SoldatMarine","def"}; bossName="Franky";       mbossName="Blueno"; }
        else if (a == "Enies Lobby")    { faction={"SoldatMarine","def"}; bossName="Lucci";        mbossName="Kaku"; }
        else if (a == "Thriller Bark")  { faction={"ZombieSimple","atk"}; bossName="GeckoMoria";   mbossName="Perona"; }
        else if (a == "Amazon Lily")    { faction={"PirateSimple","atk"}; bossName="BoaHancock";   mbossName="Sandersonia"; }
        else if (a == "Impel Down")     { faction={"SoldatMarine","def"}; bossName="Magellan";     mbossName="Saldeath"; }
        else if (a == "Marine Ford")    {
            faction = {"SoldatMarine","def"};
            bossName = "Akainu";
            mbossName = (epIdx == 2) ? "Aokiji" : "Sengoku";
        }
        else if (a == "Pantheon") {
            static const char* PANTHEON_BOSSES[] = {
                "Morgan","Baggy","Kuro","Arlong","Smoker",
                "Crocodile","Lucci","GeckoMoria","BoaHancock","Akainu"
            };
            int floor = campaign->getCurrentEpIdx();
            bossName  = PANTHEON_BOSSES[floor % 10];
            mbossName = "BossNPC";
            faction   = {"SoldatMarine", "def"};
        }
    }

    // Courbe progressive mais plafonnée pour rester jouable
    int basePV    = 400 + 30*difficulty + 5*difficulty*difficulty;
    if (basePV > 4000) basePV = 4000;
    int baseSpeed = 45  + difficulty*2;
    if (baseSpeed > 120) baseSpeed = 120;
    int baseHakiA = difficulty;
    int baseHakiO = difficulty / 2;

    // Nombre d'ennemis selon la difficulté
    int nbNormal = 3 + difficulty/4; if (nbNormal > 8) nbNormal = 8;
    int nbGuards = 2 + difficulty/5; if (nbGuards > 7) nbGuards = 7;

    if (ep && ep->isBoss) {
        int bossPV    = 2000 + 120*difficulty + 25*difficulty*difficulty;
        if (bossPV > 20000) bossPV = 20000;
        int bossSpeed = 55   + difficulty*2;
        if (bossSpeed > 130) bossSpeed = 130;
        int bossHakiR = 15 + difficulty;
        int bossHakiA = 20 + difficulty;
        int bossHakiO = 10 + difficulty;

        if (arc && arc->name == "Romance Dawn") {
            bossPV = 1500 + difficulty*80;
            if (bossPV > 2000) bossPV = 2000;
        }
        if (arc && arc->name == "Marine Ford") {
            bossPV    = 3000 + 200*difficulty + 40*difficulty*difficulty;
            if (bossPV > 35000) bossPV = 35000;
            bossSpeed = 75 + difficulty*2;
            if (bossSpeed > 145) bossSpeed = 145;
            bossHakiR = 30 + difficulty*2;
            bossHakiA = 45 + difficulty*2;
            bossHakiO = 20 + difficulty*2;
        }

        enemy->addTeamSize(nbGuards + 2); // assure enough slots for boss + guards
        addNpc(enemy, bossName, "atk", bossPV, bossSpeed, bossHakiR, bossHakiA, bossHakiO);
        for (int g = 0; g < nbGuards; g++)
            addNpc(enemy, faction.name, faction.type, basePV*2/3, baseSpeed, baseHakiA/2, baseHakiA/2, baseHakiO/2);

    } else if (ep && ep->isMiniBoss) {
        int mbPV    = 1000 + 60*difficulty + 12*difficulty*difficulty;
        if (mbPV > 10000) mbPV = 10000;
        int mbSpeed = 50  + difficulty*2;
        if (mbSpeed > 125) mbSpeed = 125;
        int mbHakiA = 10 + difficulty;
        int mbHakiO =  5 + difficulty/2;

        int nbMbGuards = 1 + difficulty/7; if (nbMbGuards > 3) nbMbGuards = 3;
        enemy->addTeamSize(nbMbGuards + 1);
        addNpc(enemy, mbossName, "atk", mbPV, mbSpeed, 10, mbHakiA, mbHakiO);
        for (int g = 0; g < nbMbGuards; g++)
            addNpc(enemy, faction.name, faction.type, basePV/2, baseSpeed, 5, baseHakiA/2, baseHakiO/2);

    } else {
        enemy->addTeamSize(nbNormal);
        for (int i = 0; i < nbNormal; i++)
            addNpc(enemy, faction.name, faction.type, basePV, baseSpeed, 5, baseHakiA, baseHakiO);
    }
}

void Game::applyRewards()
{
    Episode* ep = campaign->getCurrentEpisode();
    if (!ep) return;

    // Berries de base pour chaque combat (proportionnel à la difficulté)
    int baseReward = ep->difficulty * 60;
    if (ep->isMiniBoss) baseReward = ep->difficulty * 120;
    if (ep->isBoss)     baseReward = ep->difficulty * 200;
    player->addBerries(baseReward);

    string msg = "VICTOIRE !  +" + to_string(baseReward) + " berries";

    for (int i = 0; i < ep->nbRewards; i++)
    {
        Reward* r = ep->rewards[i];
        if (!r) continue;

        int roll = rand() % 100;
        if (roll < static_cast<int>(r->getDropChance() * 100))
        {
            if (r->getBerries() > 0)
            {
                player->addBerries(r->getBerries());
                msg += "  +" + to_string(r->getBerries()) + " berries";
            }
            if (!r->getFragmentCharName().empty() && r->getNbFragments() > 0)
            {
                player->addFragments(r->getFragmentCharName(), r->getNbFragments());
                msg += "  +" + to_string(r->getNbFragments())
                     + " frags " + r->getFragmentCharName();
            }
        }
    }

    int xpGain = ep->difficulty * 40;
    int prevLvl = player->getAccountLvl();
    player->addXP(xpGain);
    int newLvl = player->getAccountLvl();
    msg += "  +" + to_string(xpGain) + " XP";
    if (newLvl > prevLvl)
        msg += "  >> NIVEAU " + to_string(newLvl) + " ! <<";

    statusMsg = msg;
    campaign->completeCurrentEpisode();

    // Sauvegarde automatique après chaque victoire
    save();
}

// Navigation dans les listes

void Game::moveSelectionUp()
{
    if (selectedIndex > 0) selectedIndex--;
}

void Game::moveSelectionDown()
{
    // Borne supérieure selon l'état
    int max = 0;
    switch (state)
    {
        case GameState::BANK:
            max = player->getNbBank() - 1; break;
        case GameState::CAMPAIGN:
            max = campaign->getNbArcs() - 1; break;
        case GameState::CAMPAIGN_ARC:
            max = campaign->getCurrentArc()->nbEpisodes - 1; break;
        case GameState::TEAM_CHANGE:
            max = player->getNbUnlock() - 1; break;
        default:
            max = 9; break;
    }
    if (selectedIndex < max) selectedIndex++;
}

void Game::confirmSelection()
{
    switch (state)
    {
        case GameState::TEAM_CHANGE:
        {
            Character* c = player->getUnlockCharacter(selectedIndex);
            if (c) player->addToTeam(c);
            break;
        }
        default:
            break;
    }
}

// Entrées clavier

EventType Game::input(char c)
{
    switch (state)
    {
        case GameState::SAVE_SELECT:  return inputSaveSelect(c);
        case GameState::MAIN_MENU:    return inputMainMenu(c);
        case GameState::SETTINGS:     return inputSettings(c);
        case GameState::PLAY_MENU:    return inputPlayMenu(c);
        case GameState::BANK:         return inputBank(c);
        case GameState::BANK_DETAIL:  return inputBankDetail(c);
        case GameState::CAMPAIGN:     return inputCampaign(c);
        case GameState::CAMPAIGN_ARC: return inputCampaignArc(c);
        case GameState::BATTLE_PREPA: return inputBattlePrepa(c);
        case GameState::BATTLE:       return inputBattle(c);
        case GameState::TEAM:         return inputTeam(c);
        case GameState::TEAM_CHANGE:  return inputTeamChange(c);
        case GameState::SHOP:         return inputShop(c);
        default:                      return EventType::NONE;
    }
}

EventType Game::inputSaveSelect(char c)
{
    switch (c)
    {
        case 'n': return EventType::SAVE_NEW;
        case '1': return EventType::SAVE_LOAD_1;
        case '2': return EventType::SAVE_LOAD_2;
        case '3': return EventType::SAVE_LOAD_3;
        case 'r': return EventType::MAIN_MENU;  // retour au menu principal
        case 'q': return EventType::QUIT;
        default:  return EventType::NONE;
    }
}

EventType Game::inputMainMenu(char c)
{
    switch (c)
    {
        case 'j': return EventType::SAVE_SELECT;  // Jouer → choisir/charger une save
        case 'p': return EventType::SETTINGS;
        case 'q': return EventType::QUIT;
        default:  return EventType::NONE;
    }
}

EventType Game::inputSettings(char c)
{
    switch (c)
    {
        case 'f': settings->toggleFps();
                  { char b[32]; snprintf(b, sizeof(b), "FPS : %d", settings->getFps()); statusMsg = b; }
                  return EventType::NONE;
        case '+': settings->volumeUp();
                  { char b[32]; snprintf(b, sizeof(b), "Volume : %d%%", settings->getVolume()); statusMsg = b; }
                  return EventType::NONE;
        case '-': settings->volumeDown();
                  { char b[32]; snprintf(b, sizeof(b), "Volume : %d%%", settings->getVolume()); statusMsg = b; }
                  return EventType::NONE;
        case 'l': settings->brightnessUp();
                  { char b[32]; snprintf(b, sizeof(b), "Luminosite : %d%%", settings->getBrightness()); statusMsg = b; }
                  return EventType::NONE;
        case 'k': settings->brightnessDown();
                  { char b[32]; snprintf(b, sizeof(b), "Luminosite : %d%%", settings->getBrightness()); statusMsg = b; }
                  return EventType::NONE;
        case 's': settings->save("./data/settings.txt"); statusMsg = "Parametres sauvegardes."; return EventType::NONE;
        case 'm': return EventType::MAIN_MENU;
        default:  return EventType::NONE;
    }
}

EventType Game::inputPlayMenu(char c)
{
    switch (c)
    {
        case 'b': return EventType::BANK;
        case 'c': return EventType::CAMPAIGN;
        case 's': return EventType::SHOP;
        case 'm': return EventType::MAIN_MENU;
        default:  return EventType::NONE;
    }
}

EventType Game::inputBank(char c)
{
    switch (c)
    {
        case 65: moveSelectionUp();   return EventType::NONE; // flèche haut
        case 66: moveSelectionDown(); return EventType::NONE; // flèche bas
        case 10:
        case 13: return EventType::BANK_DETAIL; // entrée
        case 'm': return EventType::PLAY_MENU;
        default:  return EventType::NONE;
    }
}

EventType Game::inputBankDetail(char c)
{
    switch (c)
    {
        case 'u': return EventType::BANK_UPGRADE;
        case 'd': return EventType::BANK_UNLOCK;
        case 'r': return EventType::BANK;
        default:  return EventType::NONE;
    }
}

EventType Game::inputCampaign(char c)
{
    switch (c)
    {
        case 65: campaign->arcUp();   selectedIndex = campaign->getCurrentArcIdx(); return EventType::NONE;
        case 66: campaign->arcDown(); selectedIndex = campaign->getCurrentArcIdx(); return EventType::NONE;
        case 10:
        case 13: return EventType::CAMPAIGN_ARC;
        case 'm': return EventType::PLAY_MENU;
        default:  return EventType::NONE;
    }
}

EventType Game::inputCampaignArc(char c)
{
    switch (c)
    {
        case 65: campaign->episodeUp();   selectedIndex = campaign->getCurrentEpIdx(); return EventType::NONE;
        case 66: campaign->episodeDown(); selectedIndex = campaign->getCurrentEpIdx(); return EventType::NONE;
        case 10:
        case 13: return EventType::CAMPAIGN_EPISODE;
        case 'r': return EventType::CAMPAIGN;
        default:  return EventType::NONE;
    }
}

EventType Game::inputBattlePrepa(char c)
{
    switch (c)
    {
        case 'c': return EventType::BATTLE;
        case 'e': return EventType::TEAM_CHANGE;
        case 'r': return EventType::BATTLE_RETURN;
        case 'm': return EventType::PLAY_MENU;
        default:  return EventType::NONE;
    }
}

EventType Game::inputBattle(char c)
{
    switch (c)
    {
        case 'r': return EventType::BATTLE_RETURN;
        case 'j': return EventType::BATTLE_RETRY;
        case 'c': return EventType::BATTLE_CONTINUE;
        case 'm': return EventType::PLAY_MENU;
        default:  return EventType::NONE;
    }
}

EventType Game::inputTeam(char c)
{
    switch (c)
    {
        case 'g': return EventType::TEAM_CHANGE;
        case 'r': return EventType::PLAY;
        default:  return EventType::NONE;
    }
}

EventType Game::inputTeamChange(char c)
{
    switch (c)
    {
        case 65:  moveSelectionUp();   return EventType::NONE;
        case 66:  moveSelectionDown(); return EventType::NONE;
        case 68:  confirmSelection();  return EventType::NONE; // flèche gauche
        case 'r': return EventType::PLAY; // retour direct à BATTLE_PREPA
        default:  return EventType::NONE;
    }
}

EventType Game::inputShop(char c)
{
    // Acheter un coffre (offres permanentes [1]..[5])
    if (c >= '1' && c <= '5')
    {
        int pi = c - '1';
        const ShopOffer& offer = shop->getPermanent(pi);

        if (player->getBerries() < offer.costBerries)
        {
            statusMsg = "Berries insuffisants.";
        }
        else if (offer.isCoffre)
        {
            player->addBerries(-offer.costBerries);
            Coffre coffre(offer.coffreRar);
            Reward* r = coffre.open();
            if (r)
            {
                player->addFragments(r->getFragmentCharName(), r->getNbFragments());
                statusMsg = "Coffre : +" + to_string(r->getNbFragments())
                            + " fragments de " + r->getFragmentCharName() + " !";
                delete r;
            }
        }
        return EventType::NONE;
    }
    switch (c)
    {
        case 'm': return EventType::PLAY_MENU;
        default:  return EventType::NONE;
    }
}

// Sauvegarde

void Game::save()
{
    if (saveSlot >= 1 && saveSlot <= 3)
        SaveData::save(SAVE_PATHS[saveSlot-1], *player, *campaign);
    settings->save("./data/settings.txt");
}

bool Game::load()
{
    if (saveSlot >= 1 && saveSlot <= 3)
        return SaveData::load(SAVE_PATHS[saveSlot-1], *player, *campaign);
    return false;
}

// Accesseurs

Player*     Game::getPlayer()        const { return player;        }
Campaign*   Game::getCampaign()      const { return campaign;      }
Settings*   Game::getSettings()      const { return settings;      }
Shop*       Game::getShop()          const { return shop;          }
Battle*     Game::getCurrentBattle() const { return currentBattle; }
GameState   Game::getState()         const { return state;         }
int         Game::getSelectedIndex() const { return selectedIndex; }
string Game::getStatusMsg()     const { return statusMsg;     }
void        Game::clearStatusMsg()         { statusMsg = "";        }
void        Game::setSelectedIndex(int i)  { selectedIndex = i;     }
