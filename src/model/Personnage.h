#ifndef PERSONNAGE_H
#define PERSONNAGE_H

#include <string>
#include "Capacity.h"

using namespace std;

class Character
{
	private:
		string name;
		string type;
		string rarity;

		int pv;
		
		Capacity tabCapa[4];

	public:
};

#endif PERSONNAGE_H