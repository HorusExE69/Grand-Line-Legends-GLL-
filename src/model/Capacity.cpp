#include "Capacity.h"
#include <iostream>

using namespace std;

Capacity::Capacity()
{
	nameCapa = "";
	damage = 0;
	heal = 0;
	percentage = 0;
	isPassive = false;

	typeC.t1 = TypeC::other;
	typeC.t2 = TypeC::other;

	launcher = nullptr;
	tabEft = nullptr;

	tabTargets = nullptr;

	nbEfts = 0;
	nbTargets = 0;
}

Capacity::~Capacity()
{
	if (tabEft != nullptr)
	{
		delete[] tabEft;
		tabEft = nullptr;
	}

	if (tabTargets != nullptr)
		delete[] tabTargets;
		tabEft = nullptr;
}

// Ajouter un effet
void Capacity::addEffect(Effect* e)
{
	if(tabEft==nullptr)
	{
		tabEft = new Effect[1];
		tabEft[0] = *e;
		nbEfts = 1;
	}
	else
	{
		Effect* old = tabEft;
		Effect* tmp = new Effect[nbEfts + 1];
		for(int i=0; i < nbEfts; i++)
			tmp[i] = old[i];
		tmp[nbEfts] = *e;
		nbEfts++;
		delete[] old;
		tabEft = tmp;
	}
}

// Ajouter une cible
void Capacity::addTarget(Square* s) {
	if(tabTargets == nullptr) {
		tabTargets = new Square[1];
		tabTargets[0] = *s;
		nbTargets = 1;
	} else {
		Square* old = tabTargets;
		Square* tmp = new Square[nbTargets + 1];
		for(int i = 0; i < nbTargets; i++)
			tmp[i] = old[i];
		tmp[nbTargets] = *s;
		nbTargets++;
		delete[] old;
		tabTargets = tmp;
	}
}

// Appliquer la capacité
void Capacity::use() {
	for(int i = 0; i < nbEfts; i++) {
		Effect& e = tabEft[i];
		
		for(int j = 0; j < nbTargets; j++) {
			Square& sq = tabTargets[j];

			if(sq.inmate != nullptr) {
				Character* c = sq.inmate;

				c->applyEffect(&e);
			}
		}
	}
}

// Getters
const string& Capacity::getName() const { return nameCapa; }
int Capacity::getDamage() const { return damage; }
int Capacity::getHeal() const { return heal; }