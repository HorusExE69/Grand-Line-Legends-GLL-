/**
 * @file SaveData.cpp
 * @brief Implémentation de SaveData (sauvegarde / chargement).
 *
 * Format de la sauvegarde :
 *   PSEUDO
 *   accountLvl xp berries teamMax
 *   nbBank
 *   charName level  (×nbBank — niveau de chaque personnage)
 *   nbUnlocked
 *   charName  (×nbUnlocked)
 *   nbFragEntries
 *   charName count  (×nbFragEntries — fragments de tous les persos)
 *   arcIdx epIdx    (position courante)
 *   NB_ARCS
 *   arcCompleted ep0 ep1 ...  (×NB_ARCS — état de complétion)
 */

#include "SaveData.h"
#include "Player.h"
#include "Campaign.h"
#include "Arc.h"
#include <fstream>

using namespace std;

bool SaveData::exists(const string& path)
{
    ifstream f(path);
    return f.is_open();
}

string SaveData::getSummary(const string& path)
{
    ifstream f(path);
    if (!f.is_open()) return "";
    string pseudo;
    int accLvl = 1, xp = 0, berries = 0, teamMax = 1;
    f >> pseudo >> accLvl >> xp >> berries >> teamMax;
    if (pseudo.empty()) return "";
    char buf[64];
    snprintf(buf, sizeof(buf), "%s  |  Niv.%d  |  %d berries", pseudo.c_str(), accLvl, berries);
    return string(buf);
}

void SaveData::save(const string& path, const Player& p, const Campaign& c)
{
    ofstream f(path);
    if (!f.is_open()) return;  // Fichier inaccessible : pas de crash

    // Pseudo
    f << p.getPseudo() << "\n";

    // Compte
    f << p.getAccountLvl() << " "
      << p.getXP()         << " "
      << p.getBerries()    << " "
      << p.getTeamMax()    << "\n";

    // Niveaux des personnages (bank complète)
    f << p.getNbBank() << "\n";
    for (int i = 0; i < p.getNbBank(); i++)
    {
        Character* ch = p.getBankCharacter(i);
        f << ch->getName() << " " << ch->getLvl() << "\n";
    }

    // Personnages débloqués
    f << p.getNbUnlock() << "\n";
    for (int i = 0; i < p.getNbUnlock(); i++)
        f << p.getUnlockCharacter(i)->getName() << "\n";

    // Fragments (tous les personnages, pas seulement les débloqués)
    f << p.getNbFrag() << "\n";
    for (int i = 0; i < p.getNbFrag(); i++)
        f << p.getFragEntry(i).charName << " " << p.getFragEntry(i).count << "\n";

    // Position courante dans la campagne
    f << c.getCurrentArcIdx() << " " << c.getCurrentEpIdx() << "\n";

    // Progression arcs + épisodes
    f << NB_ARCS << "\n";
    for (int a = 0; a < NB_ARCS; a++)
    {
        const Arc* arc = c.getArc(a);
        f << arc->isCompleted();
        for (int e = 0; e < arc->getNbEpisodes(); e++)
            f << " " << arc->getEpisode(e)->isCompleted();
        f << "\n";
    }
}

bool SaveData::load(const string& path, Player& p, Campaign& c)
{
    ifstream f(path);
    if (!f.is_open()) return false;

    // Pseudo
    string pseudo;
    f >> pseudo;
    p.changePseudo(pseudo);

    // Compte
    int accLvl, xp, berries, teamMax;
    f >> accLvl >> xp >> berries >> teamMax;
    p.setAccountLvl(accLvl);
    p.setXP(xp);
    p.setBerries(berries);
    {
        int delta = teamMax - p.getTeamMax();
        if (delta > 0) p.addTeamSize(delta);
    }

    // Niveaux des personnages
    int nbCharLvl;
    f >> nbCharLvl;
    for (int i = 0; i < nbCharLvl; i++)
    {
        string charName;
        int lvl;
        f >> charName >> lvl;
        if (lvl <= 1) continue;
        for (int j = 0; j < p.getNbBank(); j++)
        {
            Character* ch = p.getBankCharacter(j);
            if (ch && ch->getName() == charName)
            {
                ch->updateLvl(lvl - 1); // updateLvl(n) ajoute n niveaux au niveau actuel (1)
                break;
            }
        }
    }

    // Personnages débloqués
    int nbUnlocked;
    f >> nbUnlocked;
    for (int i = 0; i < nbUnlocked; i++)
    {
        string charName;
        f >> charName;
        for (int j = 0; j < p.getNbBank(); j++)
        {
            Character* ch = p.getBankCharacter(j);
            if (ch && ch->getName() == charName)
            {
                p.addToUnlocked(ch);
                break;
            }
        }
    }

    // Fragments
    int nbFragEntries;
    f >> nbFragEntries;
    for (int i = 0; i < nbFragEntries; i++)
    {
        string charName;
        int count;
        f >> charName >> count;
        if (count > 0)
            p.addFragments(charName, count);
    }

    // Position courante
    int arcIdx, epIdx;
    f >> arcIdx >> epIdx;
    if (arcIdx >= 0 && arcIdx < NB_ARCS)
        c.selectArc(arcIdx);

    // Progression arcs + épisodes
    int nbArcs;
    f >> nbArcs;
    for (int a = 0; a < nbArcs && a < NB_ARCS; a++)
    {
        int arcCompleted;
        f >> arcCompleted;
        Arc* arc = c.getArc(a);
        arc->setCompleted(arcCompleted == 1);
        for (int e = 0; e < arc->getNbEpisodes(); e++)
        {
            int epCompleted;
            f >> epCompleted;
            arc->getEpisode(e)->setCompleted(epCompleted == 1);
        }
    }

    return true;
}
