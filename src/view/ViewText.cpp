/**
 * @file ViewText.cpp
 * @brief Implémentation de la vue textuelle utilisant WinTXT.
 */

#include "ViewText.h"
#include "../controller/Game.h"
#include "../model/SaveData.h"
#include "../controller/Event.h"
#include "../controller/Battle.h"
#include "../model/Campaign.h"
#include "../model/Settings.h"
#include "../model/Shop.h"
#include "../model/Family.h"
#include <cstdio>
#include <cstring>
#include <cassert>

using namespace std;

// Constructeurs

ViewText::ViewText() : win(WIN_W, WIN_H)
{
    game             = nullptr;
    running          = false;
    rewardApplied    = false;
    capaScrollOffset = 0;
}

ViewText::ViewText(Game* g) : win(WIN_W, WIN_H)
{
    assert(g != nullptr);
    game             = g;
    running          = false;
    rewardApplied    = false;
    capaScrollOffset = 0;
}

ViewText::~ViewText(void) {}

// Utilitaires

void ViewText::clearBuf()
{
    win.clear(' ');
}

void ViewText::printAt(int x, int y, const string& s)
{
    if (s.empty()) return;
    // Copier dans un buffer temporaire mutable
    char buf[WIN_W + 1];
    int  len = static_cast<int>(s.size());
    if (len > WIN_W - x) len = WIN_W - x;
    if (len <= 0) return;

    for (int i = 0; i < len; i++)
        buf[i] = s[i];
    buf[len] = '\0';
    win.print(x, y, buf);
}

void ViewText::hline(int y)
{
    char buf[WIN_W + 1];
    for (int i = 0; i < WIN_W; i++) buf[i] = '-';
    buf[WIN_W] = '\0';
    win.print(0, y, buf);
}

void ViewText::draw()
{
    win.draw();
}

// Lecture d'une chaîne en affichant les caractères saisis
string ViewText::readString(int x, int y, int maxLen)
{
    string result;
    char        c;

    draw(); // afficher le prompt

    while (true)
    {
        c = win.getCh();
        if (c == '\0') continue;
        if (c == '\n' || c == '\r') break;
        if (c == 127 || c == 8) // backspace
        {
            if (!result.empty())
            {
                result.pop_back();
                win.print(x + static_cast<int>(result.size()), y, ' ');
                draw();
            }
            continue;
        }
        if (static_cast<int>(result.size()) >= maxLen) continue;
        result += c;
        win.print(x + static_cast<int>(result.size()) - 1, y, c);
        draw();
    }
    return result;
}

// Affichage par état

void ViewText::display()
{
    if (!game) return;
    clearBuf();

    switch (game->getState())
    {
        case GameState::MAIN_MENU:    renderMainMenu();    break;
        case GameState::SETTINGS:     renderSettings();    break;
        case GameState::PLAY_MENU:    renderPlayMenu();    break;
        case GameState::BANK:         renderBank();        break;
        case GameState::BANK_DETAIL:  renderBankDetail();  break;
        case GameState::CAMPAIGN:     renderCampaign();    break;
        case GameState::CAMPAIGN_ARC: renderCampaignArc(); break;
        case GameState::BATTLE_PREPA: renderBattlePrepa(); break;
        case GameState::BATTLE:       renderBattle();      break;
        case GameState::TEAM:         renderTeam();        break;
        case GameState::TEAM_CHANGE:  renderTeamChange();  break;
        case GameState::SHOP:         renderShop();        break;
        case GameState::SAVE_SELECT:  renderSaveSelect();  break;
        case GameState::QUIT:         renderQuit();        break;
        default: break;
    }

    // Afficher le message de feedback (statusMsg) sur la dernière ligne si présent
    string msg = game->getStatusMsg();
    if (!msg.empty())
    {
        printAt(0, 23, string(WIN_W, ' ')); // effacer la ligne
        printAt(1, 23, ">> " + msg);
        game->clearStatusMsg();
    }

    draw();
}

// MENU PRINCIPAL

void ViewText::renderMainMenu()
{
    printAt(20, 0,  "=== GRAND LINE LEGENDS ===");
    hline(1);
    printAt(2,  3,  "Bienvenue dans Grand Line Legends !");
    printAt(2,  5,  "[j] Jouer");
    printAt(2,  6,  "[p] Parametres");
    printAt(2,  7,  "[q] Quitter");
    hline(22);
    printAt(2, 23,  "Utilisez les touches indiquees pour naviguer.");
}

// PARAMETRES

void ViewText::renderSettings()
{
    assert(game != nullptr);
    Settings* s = game->getSettings();

    printAt(25, 0, "=== PARAMETRES ===");
    hline(1);

    char buf[64];

    snprintf(buf, sizeof(buf), "FPS cible  : %d", s->getFps());
    printAt(2, 3, buf);
    printAt(2, 4, "[f] Basculer 30/60 fps");

    snprintf(buf, sizeof(buf), "Volume     : %d%%", s->getVolume());
    printAt(2, 6, buf);
    printAt(2, 7, "[+] Augmenter  [-] Diminuer");

    snprintf(buf, sizeof(buf), "Luminosite : %d%%", s->getBrightness());
    printAt(2, 9, buf);
    printAt(2, 10, "[l] Augmenter  [k] Diminuer");

    printAt(2, 12, "[s] Sauvegarder les parametres");
    printAt(2, 13, "[m] Retour au menu principal");

    hline(22);
}

// MENU JEU

void ViewText::renderPlayMenu()
{
    assert(game != nullptr);
    Player* p = game->getPlayer();
    char buf[64];

    printAt(25, 0, "=== GRAND LINE LEGENDS ===");
    hline(1);

    snprintf(buf, sizeof(buf), "Joueur : %s  Niveau : %d  Berries : %d",
             p->getPseudo().c_str(), p->getAccountLvl(), p->getBerries());
    printAt(2, 2, buf);

    printAt(2, 4, "[b] Bank des personnages");
    printAt(2, 5, "[c] Campagne");
    printAt(2, 6, "[s] Shop");
    printAt(2, 7, "[m] Menu principal");

    hline(22);
}

// BANK

void ViewText::renderBank()
{
    assert(game != nullptr);
    Player* p  = game->getPlayer();
    int     sel = game->getSelectedIndex();

    printAt(28, 0, "=== BANK ===");
    hline(1);
    printAt(2, 1, "Nom             | Type      | Rarete     | PV    | Lvl | D");
    hline(2);

    // Afficher les personnages (fenêtre de 18 lignes)
    int start = (sel > 9) ? sel - 9 : 0;
    int row   = 3;

    for (int i = start; i < p->getNbBank() && row < 21; i++, row++)
    {
        Character* c = p->getBankCharacter(i);
        char buf[80];
        char marker = (i == sel) ? '>' : ' ';
        char unlocked = p->isUnlocked(c->getName()) ? 'O' : '-';

        snprintf(buf, sizeof(buf), "%c %-14s | %-9s | %-10s | %-5d | %3d | %c",
                 marker,
                 c->getName().c_str(),
                 Type_DisplayString(c->typeC()).c_str(),
                 Rarity_DisplayString(c->rarC()).c_str(),
                 c->getPV(),
                 c->getLvl(),
                 unlocked);
        printAt(0, row, buf);
    }

    hline(22);
    printAt(2, 23, "[^/v] Naviguer  [Entree] Detail  [m] Menu");
}

// DETAIL PERSONNAGE

void ViewText::renderBankDetail()
{
    assert(game != nullptr);
    Player*    p   = game->getPlayer();
    int        sel = game->getSelectedIndex();
    Character* c   = p->getBankCharacter(sel);

    if (!c)
    {
        printAt(2, 5, "Aucun personnage selectionne.");
        return;
    }

    char buf[80];
    printAt(25, 0, "=== DETAIL PERSONNAGE ===");
    hline(1);

    snprintf(buf, sizeof(buf), "Nom    : %s", c->getName().c_str());                        printAt(2, 2, buf);
    snprintf(buf, sizeof(buf), "Type   : %s", Type_DisplayString(c->typeC()).c_str());      printAt(2, 3, buf);
    snprintf(buf, sizeof(buf), "Rarete : %s", Rarity_DisplayString(c->rarC()).c_str());    printAt(2, 4, buf);
    snprintf(buf, sizeof(buf), "Niveau : %d", c->getLvl());                                 printAt(2, 5, buf);
    snprintf(buf, sizeof(buf), "PV     : %d", c->getPV());                                  printAt(2, 6, buf);
    snprintf(buf, sizeof(buf), "Vitesse: %d", c->getSpeed());                               printAt(2, 7, buf);
    snprintf(buf, sizeof(buf), "Haki des Rois : %d  Armement : %d  Observation : %d",
             c->getHakiA(), c->getHakiO(), c->getHakiR());                                  printAt(2, 8, buf);

    hline(9);
    // Capacités avec défilement (6 visibles, [^/v] pour défiler)
    int nbCapa = c->getNbCapa();
    if (capaScrollOffset >= nbCapa) capaScrollOffset = (nbCapa > 0) ? nbCapa - 1 : 0;
    char capHeader[80];
    snprintf(capHeader, sizeof(capHeader), "Capacites (%d/%d) [^/v] defiler :",
             capaScrollOffset + 1, nbCapa);
    printAt(2, 10, capHeader);
    int row = 11;
    for (int i = capaScrollOffset; i < nbCapa && row < 17; i++, row++)
    {
        Capacity* cap = c->getTabCapa()[i];
        TypeC     t1  = cap->getType().t1;
        snprintf(buf, sizeof(buf), "  - %-20s [%-10s] Dmg:%5d Chance:%3d%%",
                 cap->getName().c_str(),
                 typeCToString(t1).c_str(),
                 cap->getDamage(),
                 cap->getPercentage());
        printAt(0, row, buf);
    }

    hline(17);
    bool unlocked = p->isUnlocked(c->getName());
    int  frags    = p->getFragments(c->getName());
    int  cost     = c->upgradeCost();

    snprintf(buf, sizeof(buf), "Debloque : %s | Fragments : %d / %d",
             unlocked ? "OUI" : "NON",
             frags,
             UNLOCK_FRAGMENT_COST);
    printAt(2, 18, buf);

    if (!unlocked)
        printAt(2, 19, "[d] Debloquer (100 fragments)");
    else
    {
        snprintf(buf, sizeof(buf), "[u] Ameliorer (cout : %d berries)  Berries : %d",
                 cost, p->getBerries());
        printAt(2, 19, buf);
    }

    hline(22);
    printAt(2, 23, "[r] Retour a la bank");
}

// CAMPAGNE

void ViewText::renderCampaign()
{
    assert(game != nullptr);
    Campaign* camp = game->getCampaign();
    int       sel  = camp->getCurrentArcIdx();

    printAt(27, 0, "=== CAMPAGNE ===");
    hline(1);
    printAt(2, 1, " #  Nom de l'arc              Boss                  Statut");
    hline(2);

    int start = (sel > 9) ? sel - 9 : 0;
    int row   = 3;

    for (int i = start; i < NB_ARCS && row < 21; i++, row++)
    {
        Arc*  arc    = camp->getArc(i);
        char  buf[80];
        char  marker = (i == sel) ? '>' : ' ';
        bool  locked = (i > 0 && !camp->getArc(i - 1)->completed);
        const char* statut = locked ? "[Verrouille]" :
                             (arc->completed ? "[Termine]" : "[En cours]");

        snprintf(buf, sizeof(buf), "%c %2d  %-26s %-20s %s",
                 marker, i + 1,
                 arc->name.c_str(),
                 locked ? "---" : arc->bossName.c_str(),
                 statut);
        printAt(0, row, buf);
    }

    hline(22);
    printAt(2, 23, "[^/v] Naviguer  [Entree] Choisir arc  [m] Menu");
}

// EPISODES D'UN ARC

void ViewText::renderCampaignArc()
{
    assert(game != nullptr);
    Campaign* camp = game->getCampaign();
    Arc*      arc  = camp->getCurrentArc();
    int       sel  = camp->getCurrentEpIdx();

    char buf[80];
    snprintf(buf, sizeof(buf), "=== ARC : %s ===", arc->name.c_str());
    printAt(0, 0, buf);
    hline(1);

    snprintf(buf, sizeof(buf), "Boss : %-20s  Unlock : %-15s (%d%%)",
             arc->bossName.c_str(),
             arc->unlockCharName.empty() ? "Aucun" : arc->unlockCharName.c_str(),
             static_cast<int>(arc->unlockChance * 100));
    printAt(2, 2, buf);

    if (arc->nbBanned > 0)
    {
        string banned = "Bannis : ";
        for (int i = 0; i < arc->nbBanned; i++)
        {
            if (i > 0) banned += ", ";
            banned += arc->bannedChars[i];
        }
        printAt(2, 3, banned);
    }

    hline(4);
    printAt(2, 5, " #  Nom de l'episode          Type           Statut");
    hline(6);

    int row = 7;
    for (int i = 0; i < arc->nbEpisodes && row < 20; i++, row++)
    {
        Episode*    ep     = arc->episodes[i];
        char        marker = (i == sel) ? '>' : ' ';
        bool        locked = (i > 0 && !arc->episodes[i - 1]->completed);
        const char* type;
        if      (locked)         type = "[Verrouille]";
        else if (ep->isBoss)     type = "[BOSS]      ";
        else if (ep->isMiniBoss) type = "[Mini-boss] ";
        else                     type = "[Combat]    ";

        snprintf(buf, sizeof(buf), "%c %2d  %-26s %s  %s",
                 marker, i + 1,
                 locked ? "???" : ep->name.c_str(),
                 type,
                 (!locked && ep->completed) ? "Termine" : "");
        printAt(0, row, buf);
    }

    hline(22);
    printAt(2, 23, "[^/v] Naviguer  [Entree] Lancer  [r] Arcs");
}

// PREPARATION DU COMBAT

void ViewText::renderBattlePrepa()
{
    assert(game != nullptr);
    Player*   p   = game->getPlayer();
    Campaign* camp = game->getCampaign();
    Arc*      arc  = camp->getCurrentArc();
    Episode*  ep   = camp->getCurrentEpisode();
    char      buf[80];

    snprintf(buf, sizeof(buf), "=== PREPARATION : %s - %s ===",
             arc->name.c_str(), ep ? ep->name.c_str() : "?");
    printAt(0, 0, buf);
    hline(1);

    snprintf(buf, sizeof(buf), "Difficulte : %d  |  Votre equipe : %d / %d",
             ep ? ep->difficulty : 0, p->getTeamSize(), p->getTeamMax());
    printAt(2, 2, buf);

    hline(3);
    printAt(2, 4, "Votre equipe :");
    int row = 5;
    for (int i = 0; i < p->getTeamSize() && row < 12; i++, row++)
    {
        Character* c = p->getTeamCharacter(i);
        snprintf(buf, sizeof(buf), "  %d. %-16s  PV:%5d  Spd:%3d",
                 i + 1, c->getName().c_str(), c->getPV(), c->getSpeed());
        printAt(0, row, buf);
    }
    if (p->getTeamSize() == 0)
        printAt(4, 5, "(Aucun personnage — ajoutez-en dans l'equipe)");

    hline(13);
    printAt(2, 14, "[c] Lancer le combat");
    printAt(2, 15, "[e] Gerer l'equipe");
    printAt(2, 16, "[r] Retour aux episodes");
    printAt(2, 17, "[m] Menu principal");

    hline(22);
}

// COMBAT

void ViewText::renderBattle()
{
    assert(game != nullptr);
    Battle* b = game->getCurrentBattle();
    if (!b) { printAt(2, 5, "Aucun combat en cours."); return; }

    char buf[80];
    snprintf(buf, sizeof(buf), "=== COMBAT — Tour %d ===", b->getTurn());
    printAt(0, 0, buf);
    hline(1);

    // // Grille 2x10 : ligne 0 = joueur, ligne 1 = ennemi
    // Affichage : cases [0..9] joueur sur 2 rangées, puis ennemis
    printAt(0, 2, "LIGNE 0 (Votre equipe) :");
    Player* p  = b->getPlayer();
    Player* en = b->getEnemy();

    // Afficher les 5 premières cases de chaque équipe sur une rangée
    for (int i = 0; i < 5; i++)
    {
        char cell[16];
        if (i < p->getTeamSize())
        {
            Character* c = p->getTeamCharacter(i);
            snprintf(cell, sizeof(cell), "[%d]%-8s", i,
                     c->getName().substr(0, 7).c_str());
        }
        else
            snprintf(cell, sizeof(cell), "[%d]------- ", i);
        printAt(i * 15, 3, cell);
    }
    for (int i = 0; i < p->getTeamSize() && i < 8; i++)
    {
        Character* c = p->getTeamCharacter(i);
        snprintf(buf, sizeof(buf), "%d. %-13s PV:%-5d", i, c->getName().c_str(), c->getPV());
        printAt(2, 4 + i, buf);
    }

    hline(4 + (p->getTeamSize() > 0 ? p->getTeamSize() : 1));
    int midRow = 5 + (p->getTeamSize() > 0 ? p->getTeamSize() : 1);
    printAt(0, midRow - 1, "LIGNE 1 (Ennemis) :");

    for (int i = 0; i < en->getTeamSize() && i < 8; i++)
    {
        Character* c = en->getTeamCharacter(i);
        snprintf(buf, sizeof(buf), "%d. %-13s PV:%-5d", i, c->getName().c_str(), c->getPV());
        printAt(2, midRow + i, buf);
    }

    int logStart = midRow + (en->getTeamSize() > 0 ? en->getTeamSize() : 1) + 1;
    if (logStart > 15) logStart = 15;
    hline(logStart - 1);
    printAt(2, logStart - 1, "Actions :");

    int logRow = logStart;
    int nb = b->getNbLog();
    int first = (nb > 5) ? nb - 5 : 0;
    for (int i = first; i < nb && logRow < 21; i++, logRow++)
    {
        const BattleLog& l = b->getLog(i);
        snprintf(buf, sizeof(buf), "  %-10s->%-10s [%s] -%d",
                 l.attacker.c_str(), l.target.c_str(),
                 l.capa.c_str(), l.value);
        printAt(0, logRow, buf);
    }

    if (b->isOver())
    {
        // Appliquer les récompenses une seule fois quand le joueur gagne
        if (!rewardApplied)
        {
            if (b->getWinner() == b->getPlayer())
                game->applyRewards();
            rewardApplied = true;
        }

        Player* winner = b->getWinner();
        snprintf(buf, sizeof(buf), ">>> %s a gagne ! <<<",
                 winner ? winner->getPseudo().c_str() : "Egalite");
        printAt(20, 21, buf);
    }

    hline(22);
    if (b->isOver())
    {
        printAt(2, 23, "[r] Retour aux episodes  [m] Menu");
    }
    else
        printAt(2, 23, "Appuyez sur une touche pour le tour suivant...");
}

// EQUIPES

void ViewText::renderTeam()
{
    assert(game != nullptr);
    Battle* b = game->getCurrentBattle();
    printAt(28, 0, "=== EQUIPES ===");
    hline(1);

    Player* p  = game->getPlayer();
    Player* en = b ? b->getEnemy() : nullptr;

    printAt(2, 2, "VOTRE EQUIPE");
    int row = 3;
    char buf[80];
    for (int i = 0; i < p->getTeamSize() && row < 11; i++, row++)
    {
        Character* c = p->getTeamCharacter(i);
        snprintf(buf, sizeof(buf), "  %-16s PV:%5d Spd:%3d",
                 c->getName().c_str(), c->getPV(), c->getSpeed());
        printAt(0, row, buf);
    }

    if (en)
    {
        hline(11);
        printAt(2, 12, "EQUIPE ENNEMIE");
        row = 13;
        for (int i = 0; i < en->getTeamSize() && row < 21; i++, row++)
        {
            Character* c = en->getTeamCharacter(i);
            snprintf(buf, sizeof(buf), "  %-16s PV:%5d Spd:%3d",
                     c->getName().c_str(), c->getPV(), c->getSpeed());
            printAt(0, row, buf);
        }
    }

    hline(22);
    printAt(2, 23, "[g] Modifier equipe  [r] Retour");
}

// MODIFICATION D'EQUIPE

void ViewText::renderTeamChange()
{
    assert(game != nullptr);
    Player* p   = game->getPlayer();
    int     sel = game->getSelectedIndex();
    char    buf[80];

    printAt(25, 0, "=== MODIFIER L'EQUIPE ===");
    hline(1);

    snprintf(buf, sizeof(buf), "Equipe actuelle (%d/%d) :", p->getTeamSize(), p->getTeamMax());
    printAt(2, 2, buf);
    int row = 3;
    for (int i = 0; i < p->getTeamSize() && row < 7; i++, row++)
    {
        Character* c = p->getTeamCharacter(i);
        snprintf(buf, sizeof(buf), "  %d. %-16s PV:%5d", i+1, c->getName().c_str(), c->getPV());
        printAt(0, row, buf);
    }

    hline(8);
    printAt(2, 9, "Personnages disponibles :");
    hline(10);
    row = 11;
    int start = (sel > 8) ? sel - 8 : 0;

    for (int i = start; i < p->getNbUnlock() && row < 21; i++, row++)
    {
        Character* c     = p->getUnlockCharacter(i);
        char       mark  = (i == sel) ? '>' : ' ';
        snprintf(buf, sizeof(buf), "%c %-16s %-9s PV:%5d Spd:%3d",
                 mark, c->getName().c_str(),
                 Type_DisplayString(c->typeC()).c_str(),
                 c->getPV(), c->getSpeed());
        printAt(0, row, buf);
    }

    hline(22);
    printAt(2, 23, "[^/v] Naviguer  [<-] Ajouter  [r] Retour");
}

// SHOP

void ViewText::renderShop()
{
    assert(game != nullptr);
    Shop*   shop = game->getShop();
    Player* p    = game->getPlayer();
    char    buf[80];

    printAt(29, 0, "=== SHOP ===");
    hline(1);

    snprintf(buf, sizeof(buf), "Vos berries : %d", p->getBerries());
    printAt(2, 2, buf);
    hline(3);

    printAt(2, 4, "Coffres disponibles (contenu aleatoire) :");
    hline(5);

    for (int i = 0; i < NB_SHOP_PERMANENT; i++)
    {
        const ShopOffer& off = shop->getPermanent(i);
        snprintf(buf, sizeof(buf), "[%d] %-40s — %d berries",
                 i + 1,
                 off.label.c_str(),
                 off.costBerries);
        printAt(2, 6 + i * 2, buf);
    }

    hline(13);
    printAt(2, 14, "Chaque coffre contient des berries et parfois des fragments.");
    printAt(2, 15, "Coffre Commun  : berries + chance perso commun/rare");
    printAt(2, 16, "Coffre Rare    : plus de berries + chance perso rare/SR");
    printAt(2, 17, "Coffre Epique  : beaucoup de berries + chance perso epique");

    hline(22);
    printAt(2, 23, "[1][2][3] Acheter un coffre  [m] Menu principal");
}

// SELECTION DE SAUVEGARDE

void ViewText::renderSaveSelect()
{
    printAt(13, 0, "=== CHOISIR UNE SAUVEGARDE ===");
    hline(1);
    printAt(2, 3, "[n] Nouvelle partie");
    hline(4);
    printAt(2, 5, "Charger une sauvegarde existante :");
    hline(6);

    char buf[WIN_W + 1];
    for (int i = 0; i < 3; i++)
    {
        string summary = SaveData::getSummary(SAVE_PATHS[i]);
        if (summary.empty())
            snprintf(buf, sizeof(buf), "[%d] Emplacement %d : Vide", i + 1, i + 1);
        else
            snprintf(buf, sizeof(buf), "[%d] Emplacement %d : %s", i + 1, i + 1,
                     summary.c_str());
        printAt(2, 8 + i * 2, buf);
    }

    hline(15);
    printAt(2, 16, "[r] Retour au menu principal");
    hline(22);
}

// QUITTER

void ViewText::renderQuit()
{
    printAt(20, 10, "Sauvegarde... Au revoir !");
    draw();
}

// Boucle principale

void ViewText::run()
{
    assert(game != nullptr);
    running = true;

    // // Demande du pseudo
    clearBuf();
    printAt(15, 5, "=== GRAND LINE LEGENDS ===");
    hline(6);
    printAt(2,  8, "Nouveau joueur ou chargement ?");
    printAt(2, 10, "[n] Nouvelle partie");
    printAt(2, 11, "[c] Charger sauvegarde");
    draw();

    // Attendre le choix
    char choice = '\0';
    while (choice != 'n' && choice != 'c')
        choice = win.getCh();

    if (choice == 'c' && game->load())
    {
        // Sauvegarde chargée avec succès
        clearBuf();
        printAt(15, 10, "Sauvegarde chargee !");
        draw();
        win.pause();
    }
    else
    {
        // Nouvelle partie : demander le pseudo
        clearBuf();
        printAt(2, 10, "Entrez votre pseudo : ");
        string pseudo = readString(24, 10, 16);
        game->getPlayer()->changePseudo(pseudo);

        // Débloquer Luffy par défaut
        if (game->getPlayer()->getNbBank() > 0)
        {
            game->getPlayer()->addToUnlocked(game->getPlayer()->getBankCharacter(0));
            game->getPlayer()->addToTeam(game->getPlayer()->getUnlockCharacter(0));
        }
    }

    // // Boucle de jeu
    display();

    while (running)
    {
        char c = win.getCh();
        if (c == '\0') continue;

        // Gestion spéciale de l'état BATTLE (touche → tour suivant)
        if (game->getState() == GameState::BATTLE)
        {
            Battle* b = game->getCurrentBattle();
            if (b && !b->isOver())
            {
                b->playNextTurn();
                display();
                continue;
            }
            // Combat terminé : attendre une touche de navigation ([r]/[m])
        }

        GameState prevState = game->getState();

        // Dans BANK_DETAIL, les flèches défilent les capacités plutôt que de naviguer
        if (game->getState() == GameState::BANK_DETAIL)
        {
            if (c == 65) // flèche haut
            {
                if (capaScrollOffset > 0) capaScrollOffset--;
                display();
                continue;
            }
            if (c == 66) // flèche bas
            {
                Character* ch = game->getPlayer()->getBankCharacter(game->getSelectedIndex());
                if (ch && capaScrollOffset < ch->getNbCapa() - 1) capaScrollOffset++;
                display();
                continue;
            }
        }
        // Réinitialiser le scroll quand on entre dans le détail
        if (game->getState() != GameState::BANK_DETAIL) capaScrollOffset = 0;

        Event ev(game->input(c));
        game->update(&ev);

        // Réinitialiser rewardApplied au démarrage d'un nouveau combat
        if (prevState != GameState::BATTLE && game->getState() == GameState::BATTLE)
            rewardApplied = false;

        if (game->getState() == GameState::QUIT)
        {
            display(); // afficher "au revoir"
            running = false;
        }
        else
        {
            display();
        }
    }
}

bool ViewText::isRunning() const { return running; }
