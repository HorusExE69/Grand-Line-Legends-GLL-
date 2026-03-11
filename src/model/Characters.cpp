#include "Characters.h"
#include "Capacity.h"
#include "Effects.h"
#include <fstream>
#include <iostream>
#include <string>

using namespace std;

// CONSTRUCTEURS

// Constructeur par défaut
Character::Character(void)
{
	name = "";
	type = Type_::atk_;
	rarity = Rarity::c;
	pv = 0;
	speed = 0;
	lvl = 1;
	tabCapa = nullptr;
	nbCapa = 0;
	maxCapa = 0;
	tabEffects = nullptr;
	nbEffects = 0;
	maxEffects = 3;
	pos = nullptr;
	hakiA = 0;
	hakiO = 0;
	hakiR = 0;
}

// Constructeur depuis fichier (CSV)
Character::Character(string line, string path)
{
	nbCapa = 0;
	maxCapa = 10;
	tabCapa = new Capacity*[maxCapa];

	size_t cur = 0;
	string token;

	cur = line.find(',');
	line = line.substr(cur + 1);

	// name
	cur = line.find(',');
	name = line.substr(0, cur);
	line = line.substr(cur + 1);

	// type
	cur = line.find(',');
	type = stringToType_(line.substr(0, cur));
	line = line.substr(cur + 1);

	// rarity
	cur = line.find(',');
	rarity = stringToRarity(line.substr(0, cur));
	line = line.substr(cur + 1);

	// pv
	cur = line.find(',');
	pv = stoi(clean(line.substr(0, cur)));
	line = line.substr(cur + 1);

	// speed
	cur = line.find(',');
	speed = stoi(clean(line.substr(0, cur)));
	line = line.substr(cur + 1);


	// hakiR
	cur = line.find(',');
	hakiR = stoi(clean(line.substr(0, cur)));
	line = line.substr(cur + 1);

	// hakiA
	cur = line.find(',');
	hakiA = stoi(clean(line.substr(0, cur)));
	line = line.substr(cur + 1);

	// hakiO
	if (!line.empty())
		hakiO = stoi(line);
	else
		hakiO = 0;

	loadCapacities(path);
	

	lvl = 1;
	tabEffects = nullptr;
	nbEffects = 0;
	maxEffects = 3;
	pos = nullptr;
}

// Destructeur
Character::~Character(void)
{
	if (tabCapa != nullptr)
		delete[] tabCapa;
	if(pos != nullptr)
		delete pos;
	if(tabEffects != nullptr)
		delete[] tabEffects;
}

// FONCTIONS MEMBRE

void Character::loadCapacities(string pathCapaFolder)
{
	string path = pathCapaFolder + name + ".csv";
	cout << "Chargement des capacites de " << name << " depuis " << path << "...\n";
	// string path= "./data/capacities/Sanji.csv";

	ifstream file(path);
	if(!file.is_open()) cout << "Impossible d'ouvrir" << path << endl;

	string line;

	getline(file,line); // header

	while(getline(file,line))
	{
		Capacity* c = new Capacity(line);
		addToCapa(c);
	}
}

void Character::applyEffect(Effect* e)
{
	if (nbEffects < maxEffects)
	{
		tabEffects[nbEffects] = e;
		nbEffects++;

		switch(e->getType())
		{
			case EffectType::Damage:
				pv -= e->getValue();
				if (pv < 0) pv = 0;
				break;

			case EffectType::Heal:
				pv += e->getValue();
				break;

			case EffectType::Swap:
				if (e->getExtraTarget1() && e->getExtraTarget2())
				{
					Square* s1 = e->getExtraTarget1()->pos;
					Square* s2 = e->getExtraTarget2()->pos;
					if (s1 && s2) s1->swap(s2);
				}
				break;

			default:
				break;
		}
	}
}

void Character::addToCapa(Capacity* c)
{
	if (c == nullptr) return;

	if (nbCapa >= maxCapa)
	{
		int newMaxCapa = maxCapa * 2;
		Capacity** newTabCapa = new Capacity*[newMaxCapa];
		for (int i = 0; i < nbCapa; i++) newTabCapa[i] = tabCapa[i];
		delete[] tabCapa;
		tabCapa = newTabCapa;
		maxCapa = newMaxCapa;
	}

	tabCapa[nbCapa++] = c;
}

void Character::showCapa() const
{
	if(nbCapa == 0 || tabCapa == nullptr)
	{
		std::cout << name << " n'a aucune capacite.\n";
		return;
	}

	std::cout << "Capacites de " << name << " :\n";

	for(int i = 0; i < nbCapa; i++)
	{
		Capacity* c = tabCapa[i];

		std::cout << "- " << c->getName();

		if(c->getDamage() > 0)
			std::cout << " | Damage: " << c->getDamage();

		if(c->getHeal() > 0)
			std::cout << " | Heal: " << c->getHeal();

		std::cout << " | Chance: " << c->getPercentage() << "%";

		std::cout << std::endl;
	}
}


// Accesseurs

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

string Character::getName(void) { return name; }
int Character::getPV(void) { return pv; }

// Update niveau
void Character::updateLvl(int nbLvl)
{
	lvl += nbLvl;
	pv += nbLvl * 50;
	speed += nbLvl * 2;

	if (speed > 125)
		speed = 125;
}
