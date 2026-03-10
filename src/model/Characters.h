#ifndef PERSONNAGE_H
#define PERSONNAGE_H

#include <string>
#include "Capacity.h"
#include "Squares.h"
#include "Utils.h"

class Capacity;

class Character
{
	private:
		std::string name;

		Type_ type;
		
		Rarity rarity;

		Square* pos;

		int pv;
		int speed;
		int hakiR;
		int hakiO;
		int hakiA;
		int lvl;
		
		Capacity** tabCapa;

		Effect** tabEffects;
		int nbEffects;
		int maxEffects;

	public:
		Character(void);
		Character(std::string n, Type_ tc, Rarity rar, int basePv, int baseSpeed, int hR=0, int hO=0, int hA=0, Capacity** tabC=nullptr, Square* xy=nullptr, Effect** tabE=nullptr, int nbE=0, int maxE=3);
		Character(std::ifstream& file);
		~Character(void);

		Type_ typeC(void);
		bool isSup(void);
		bool isDef(void);
		bool isInt(void);
		bool isAtk(void);
		bool isSnp(void);
		bool isMag(void);

		Rarity rarC(void);
		bool isC(void);
		bool isR(void);
		bool isSR(void);
		bool isE(void);
		bool isL(void);
		bool isM(void);

		void updateLvl(int nbLvl);
		std::string getName(void);
		int getPV(void);

		void applyEffect(Effect* e);
};

#endif