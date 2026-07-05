#include "Capacity.h"
#include "Utils.h"
#include <iostream>
#include <fstream>

using namespace std;

Capacity::Capacity()
{
	nameCapa = "";
	damage = 0;
	heal = 0;
	percentage = 0;
	isPassive = false;
	activated = false;

	typeC.t1 = TypeC::other;
	typeC.t2 = TypeC::other;

	launcher = nullptr;
	eft = nullptr;

	tabTargets = nullptr;

	nbTargets = 0;
}

Capacity::Capacity(string line)
{
	nameCapa = "";
	damage = 0;
	heal = 0;
	percentage = 0;
	isPassive = false;
	activated = false;

	typeC.t1 = TypeC::other;
	typeC.t2 = TypeC::other;

	launcher = nullptr;
	eft = nullptr;

	tabTargets = nullptr;

	nbTargets = 0;

	if(line.empty())
		return;

	size_t cur = 0;
	string token;

	cur = line.find(',');
	line = line.substr(cur + 1);
	cur = line.find(',');
	line = line.substr(cur + 1);

	// name
	cur = line.find(',');
	nameCapa = line.substr(0, cur);
	line = line.substr(cur + 1);

	// type1
	cur = line.find(',');
	typeC.t1 = stringToTypeC(line.substr(0, cur));
	line = line.substr(cur + 1);

	// type2
	cur = line.find(',');
	typeC.t2 = stringToTypeC(line.substr(0, cur));
	line = line.substr(cur + 1);

	// damage
	cur = line.find(',');
	damage = stoi(clean(line.substr(0, cur)));
	line = line.substr(cur + 1);

	// heal
	cur = line.find(',');
	heal = stoi(clean(line.substr(0, cur)));
	line = line.substr(cur + 1);

	// // targetType
	cur = line.find(',');
	TargetType targetType = stringToTargetType(line.substr(0, cur));
	line = line.substr(cur + 1);


	// nbTargets
	cur = line.find(',');
	nbTargets = stoi(clean(line.substr(0, cur)));
	line = line.substr(cur + 1);

	cur = line.find(',');
	line = line.substr(cur + 1);
	cur = line.find(',');
	line = line.substr(cur + 1);

	// effet
	cur = line.find(',');
	EffectType effectType = stringToEffect(line.substr(0, cur));
	line = line.substr(cur + 1);

	
	// Pour les soins, utiliser la valeur heal et non damage (damage=0 pour les soins)
	int effVal = (effectType == EffectType::Heal) ? heal : damage;
	eft = new Effect(effectType, targetType, effVal, 1, nullptr, nullptr);

	// pourcentage
	if (!line.empty())
		percentage = stoi(line);
	else
		percentage = 0;

	if (percentage >= 100) isPassive = true;
	else isPassive = false;
}

Capacity::~Capacity()
{
	if (eft != nullptr)
	{
		delete eft;
		eft = nullptr;
	}

	if (tabTargets != nullptr)
	{
		delete[] tabTargets;
		tabTargets = nullptr;
	}
}

// Ajouter une cible
void Capacity::addTarget(Square* s)
{
	if(tabTargets == nullptr)
	{
		tabTargets = new Square*[1];
		tabTargets[0] = s;
		nbTargets = 1;
	}
	else
	{
		Square** old = tabTargets;
		Square** tmp = new Square*[nbTargets + 1];

		for(int i = 0; i < nbTargets; i++)
			tmp[i] = old[i];

		tmp[nbTargets] = s;
		nbTargets++;

		delete[] old;
		tabTargets = tmp;
	}
}


// Appliquer la capacité
void Capacity::use()
{
	if(nbTargets <= 0 || tabTargets == nullptr)
		return;

	for(int i = 0; i < nbTargets; i++)
	{
		Square* sq = tabTargets[i];
		if(sq == nullptr || sq->getInmate() == nullptr)
			continue;

		Character* c = sq->getInmate();
		c->applyEffect(eft);
	}

	delete[] tabTargets;
	tabTargets = nullptr;
	nbTargets = 0;
}



// Getters
const string& Capacity::getName() const { return nameCapa; }
int Capacity::getDamage() const { return damage; }
int Capacity::getHeal() const { return heal; }
int Capacity::getPercentage() const { return percentage; }
TupleTC Capacity::getType() const { return typeC; }
bool Capacity::getIsPassive() const { return isPassive; }
bool Capacity::getActivated() const { return activated; }
Effect* Capacity::getEffect() const { return eft; }

void Capacity::setLauncher(Square* s) { launcher = s; }
