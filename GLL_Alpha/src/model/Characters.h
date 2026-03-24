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
		int nbCapa;
		int maxCapa;

		Effect** tabEffects;
		int nbEffects;
		int maxEffects;

	public:
		int resistValue;
		int bleedingValue;
		bool isBleeding;
		bool stunned;

	public:
		Character(void);
		Character(std::string line, std::string pathCapaFolder);
		~Character(void);

		void loadCapacities(std::string path);

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
		Capacity *chooseCapa();
		void applyPassives();

		// void applyBuff(Effect* e);
		// void applyDebuff(Effect* e);


		void addToCapa(Capacity* c);

		int getNbCapa() const;
		Capacity** getTabCapa() const;
		int getSpeed() const;
};

#endif