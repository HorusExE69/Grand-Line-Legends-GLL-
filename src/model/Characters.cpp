#include "Characters.h"
#include <fstream>
#include <iostream>

using namespace std;

// CONSTRUCTEURS

// Constructeur par défaut
Character::Character(void)
{
	name = "";
	type = "";
	rarity = "";
	pv = 0;
	speed = 0;
	lvl = 1;
	tabCapa = nullptr;
}

// Constructeur avec paramètres
Character::Character(string n, Type_ tc, Rarity rar, int basePv, int baseSpeed, Capacity* tabC)
{
	name = n;
	type = tc;
	rarity = rar;
	pv = basePv;
	speed = baseSpeed;
	lvl = 1;
	tabCapa = tabC;
}

// Constructeur depuis fichier (CSV)
Character::Character(ifstream file)
{
	string line;
	if (getline(file, line))
	{
		size_t pos = 0;
		string token;

		pos = line.find(',');
		line = line.substr(pos + 1);

		// name
		pos = line.find(',');
		name = line.substr(0, pos);
		line = line.substr(pos + 1);

		// type
		pos = line.find(',');
		type = stringToType_(line.substr(0, pos));
		line = line.substr(pos + 1);

		// rarity
		pos = line.find(',');
		rarity = stringToRarity(line.substr(0, pos));
		line = line.substr(pos + 1);

		// pv
		pos = line.find(',');
		pv = stoi(line.substr(0, pos));
		line = line.substr(pos + 1);

		// speed
		if (!line.empty())
			speed = stoi(line);
		else
			speed = 100;

		lvl = 1;
		tabCapa = nullptr;
	}
}

// Destructeur
Character::~Character(void)
{
	if (tabCapa != nullptr)
		delete[] tabCapa;
}

// FONCTIONS
Type_ Character::typeC(void)
{
	return type;
}

bool Character::isSup(void) { return type == Type_::sup_; }
bool Character::isDef(void) { return type == Type_::def_; }
bool Character::isInt(void) { return type == Type_::int_; }
bool Character::isAtk(void) { return type == Type_::atk_; }
bool Character::isSnp(void) { return type == Type_::snp_; }
bool Character::isMag(void) { return type == Type_::mag_; }

// ===== Fonctions de rareté =====
Rarity Character::rarC(void)
{
	return rarity;
}

bool Character::isC(void) { return rarity == Rarity::c; }
bool Character::isR(void) { return rarity == Rarity::r; }
bool Character::isSR(void) { return rarity == Rarity::sr; }
bool Character::isE(void) { return rarity == Rarity::e; }
bool Character::isL(void) { return rarity == Rarity::l; }
bool Character::isM(void) { return rarity == Rarity::m; }

// ===== Update niveau =====
void Character::updateLvl(int nbLvl)
{
	lvl += nbLvl;
	pv += nbLvl * 50;
	speed += nbLvl * 2;

	if (speed > 125)
		speed = 125;
}

Type_ stringToType_(const string& str)
{
    if (str == "atk") return Type_::atk_;
    if (str == "def") return Type_::def_;
    if (str == "sup") return Type_::sup_;
    if (str == "int") return Type_::int_;
    if (str == "snp") return Type_::snp_;
    if (str == "mag") return Type_::mag_;
    return Type_::atk_; // valeur par défaut
}

Rarity stringToRarity(const string& str)
{
    if (str == "c") return Rarity::c;
    if (str == "r") return Rarity::r;
    if (str == "sr") return Rarity::sr;
    if (str == "e") return Rarity::e;
    if (str == "l") return Rarity::l;
    if (str == "m") return Rarity::m;
    return Rarity::c; // valeur par défaut
}