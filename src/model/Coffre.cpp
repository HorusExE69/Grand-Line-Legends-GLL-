/**
 * @file Coffre.cpp
 * @brief Implémentation de Coffre.
 *
 * Intervalles de fragments par niveau de coffre :
 *   Rare      : commun 50-100  | rare 20-49
 *   SR        : commun 100-200 | rare 50-100   | SR 20-49
 *   Épique    : commun 200-300 | rare 100-200  | SR 50-100   | épique 20-49
 *   Légendaire: commun 300-400 | rare 200-300  | SR 100-200  | épique 50-100  | lég. 20-49
 *   Mythique  : commun 400-500 | rare 300-400  | SR 200-300  | épique 100-200 | lég. 50-100 | myth. 20-49
 */

#include "Coffre.h"
#include <cstdlib>
#include <cassert>

using namespace std;

Coffre::Coffre(Rarity r) : rarity(r), opened(false) {}

Coffre::~Coffre() {}

Reward* Coffre::open()
{
    assert(!opened);
    opened = true;
    int roll = rand() % 100;

    static const char* POOL_C[]  = {"Luffy","Zoro","Sanji","Brook","Lucci","Killer","Usopp","Franky","Vivi","Nami","Koby","BonClay","Baggy","Bepo","Chopper"};
    static const char* POOL_R[]  = {"Bege","Urouge","Momonosuke","Burgess","Carrot","VanAugur","Bonney","Appo","Ivankov","XDrake","GeckoMoria","Izo","Smoker"};
    static const char* POOL_SR[] = {"Jinbe","Robin","Crocodile","Kuma","Kid","Yamato","BoaHancock","Hawkins","Ace","Shiliew","Yasopp","Doflamingo"};
    static const char* POOL_E[]  = {"Vegapunk","DoggyBroggy","Zunesha","Emeth","Fujitora","King","Katakuri","BenBeckman","Ener","Ryokugyu","Marco"};
    static const char* POOL_L[]  = {"Rayleigh","Law","Sengoku","Akainu","Kizaru","Aokiji","Sabo","Crocus"};
    static const char* POOL_M[]  = {"Shanks","Garp","Dragon","Mihawk","Kaido","BigMom","BarbeNoire","BarbeBlanche"};
    static const int SZ_C=15, SZ_R=13, SZ_SR=12, SZ_E=11, SZ_L=8, SZ_M=8;

    string charName;
    int frags = 0;

    switch (rarity)
    {
        case Rarity::c:
        case Rarity::r: {
            // Coffre Rare : 30% commun (50-100 frags) | 70% rare (20-49 frags)
            if (roll < 30) { charName = POOL_C[rand()%SZ_C]; frags = rand()%51 + 50; }
            else           { charName = POOL_R[rand()%SZ_R]; frags = rand()%30 + 20; }
            break;
        }
        case Rarity::sr: {
            // Coffre SR : 20% commun (100-200) | 35% rare (50-100) | 45% SR (20-49)
            if      (roll < 20) { charName = POOL_C [rand()%SZ_C];  frags = rand()%101 + 100; }
            else if (roll < 55) { charName = POOL_R [rand()%SZ_R];  frags = rand()%51  + 50;  }
            else                { charName = POOL_SR[rand()%SZ_SR]; frags = rand()%30  + 20;  }
            break;
        }
        case Rarity::e: {
            // Coffre Épique : 15% commun (200-300) | 25% rare (100-200) | 30% SR (50-100) | 30% épique (20-49)
            if      (roll < 15) { charName = POOL_C [rand()%SZ_C];  frags = rand()%101 + 200; }
            else if (roll < 40) { charName = POOL_R [rand()%SZ_R];  frags = rand()%101 + 100; }
            else if (roll < 70) { charName = POOL_SR[rand()%SZ_SR]; frags = rand()%51  + 50;  }
            else                { charName = POOL_E [rand()%SZ_E];  frags = rand()%30  + 20;  }
            break;
        }
        case Rarity::l: {
            // Coffre Légendaire : 10% commun (300-400) | 15% rare (200-300) | 25% SR (100-200) | 25% épique (50-100) | 25% légendaire (20-49)
            if      (roll < 10) { charName = POOL_C [rand()%SZ_C];  frags = rand()%101 + 300; }
            else if (roll < 25) { charName = POOL_R [rand()%SZ_R];  frags = rand()%101 + 200; }
            else if (roll < 50) { charName = POOL_SR[rand()%SZ_SR]; frags = rand()%101 + 100; }
            else if (roll < 75) { charName = POOL_E [rand()%SZ_E];  frags = rand()%51  + 50;  }
            else                { charName = POOL_L [rand()%SZ_L];  frags = rand()%30  + 20;  }
            break;
        }
        case Rarity::m: {
            // Coffre Mythique : 5% commun (400-500) | 10% rare (300-400) | 15% SR (200-300) | 20% épique (100-200) | 25% légendaire (50-100) | 25% mythique (20-49)
            if      (roll <  5) { charName = POOL_C [rand()%SZ_C];  frags = rand()%101 + 400; }
            else if (roll < 15) { charName = POOL_R [rand()%SZ_R];  frags = rand()%101 + 300; }
            else if (roll < 30) { charName = POOL_SR[rand()%SZ_SR]; frags = rand()%101 + 200; }
            else if (roll < 50) { charName = POOL_E [rand()%SZ_E];  frags = rand()%101 + 100; }
            else if (roll < 75) { charName = POOL_L [rand()%SZ_L];  frags = rand()%51  + 50;  }
            else                { charName = POOL_M [rand()%SZ_M];  frags = rand()%30  + 20;  }
            break;
        }
    }

    return new Reward(0, charName, frags, 1.0f);
}

bool   Coffre::isOpened()  const { return opened; }
Rarity Coffre::getRarity() const { return rarity; }
