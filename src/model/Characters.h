#ifndef PERSONNAGE_H
#define PERSONNAGE_H

#include <string>
#include "Capacity.h"
#include "Squares.h"


enum class Rarity
{
	c,
	r,
	sr,
	e,
	l,
	m
};

enum class Type_
{
	snp_,
	int_,
	mag_,
	atk_,
	def_,
	sup_
};

Type_ stringToType_(const std::string& str);
Rarity stringToRarity(const std::string& str);
std::string Type_Tostring(const Type_& typ);
std::string RarityTostring(const Rarity& rar);

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