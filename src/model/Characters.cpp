#include "Characters.h"
#include "Capacity.h"
#include "Effects.h"
#include "Utils.h"
#include "../controller/Game.h"
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
	if(tabEffects != nullptr)
		delete[] tabEffects;
	pos = nullptr;
}

// FONCTIONS MEMBRE

void Character::loadCapacities(string pathCapaFolder)
{
	string path = pathCapaFolder + name + ".csv";
	ifstream file(path);
	// if(!file.is_open()) return;

	string line;

	getline(file,line);

	while(getline(file,line))
	{
		if(line.empty())
			continue;
		Capacity* c = new Capacity(line);
		addToCapa(c);
	}
}

void Character::applyEffect(Effect* e)
{
	bool canStore = nbEffects < maxEffects;
	if (canStore)
	{
		if (tabEffects == nullptr)
			tabEffects = new Effect*[maxEffects];

		tabEffects[nbEffects++] = e;
	}

	switch(e->getType())
	{
		case EffectType::Damage:
			pv -= e->getValue();
			if (pv < 0) pv = 0;
			break;

		case EffectType::Heal:
			pv += e->getValue();
			break;

		case EffectType::Buff:
			// applyBuff(e);
			break;

		case EffectType::Debuff:
			// applyDebuff(e);
			break;

		case EffectType::Resist:
			resistValue += e->getValue();
			break;

		case EffectType::Stun:
			stunned = true;
			break;

		case EffectType::Bleeding:
			bleedingValue = e->getValue();
			break;

		case EffectType::Push:
		case EffectType::Pull:
			if (e->getExtraTarget1())
			{
				Square* s1 = pos;
				Square* s2 = e->getExtraTarget1()->pos;
				if(s1 && s2)
					s1->swap(s2);
			}
			break;

		case EffectType::Swap:
			if (e->getExtraTarget1() && e->getExtraTarget2())
			{
				Square* s1 = e->getExtraTarget1()->pos;
				Square* s2 = e->getExtraTarget2()->pos;
				if(s1 && s2)
					s1->swap(s2);
			}
			break;

		case EffectType::Other:
			break;
	}
}

void Character::applyPassives()
{
	if(nbCapa == 0 || tabCapa == nullptr)
		return;

	for(int i = 0; i < nbCapa; i++)
	{
		Capacity* c = tabCapa[i];

		if(!c || (!c->getIsPassive() && c->getActivated()))
			continue;

		c->use();
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


Capacity* Character::chooseCapa()
{
	if(nbCapa == 0 || tabCapa == nullptr)
		return nullptr;

	int total = 0;
	for(int i = 0; i < nbCapa; i++)
	{
		if(!tabCapa[i] || tabCapa[i]->getIsPassive())
			continue;

		total += tabCapa[i]->getPercentage();
	}

	if(total == 0)
		return nullptr;

	int roll = rand()%total;

	int sum = 0;

	for(int i = 0; i < nbCapa; i++)
	{
		if(!tabCapa[i] || tabCapa[i]->getIsPassive())
			continue;
		
		sum += tabCapa[i]->getPercentage();
		if(roll < sum)
			return tabCapa[i];
	}

	return nullptr;
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
int Character::getNbCapa() const { return nbCapa; }
Capacity** Character::getTabCapa() const { return tabCapa; }

// Update niveau
void Character::updateLvl(int nbLvl)
{
	lvl += nbLvl;
	pv += nbLvl * 50;
	speed += nbLvl * 2;

	if (speed > 125)
		speed = 125;
}