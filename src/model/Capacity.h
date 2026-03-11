#ifndef CAPACITY_H
#define CAPACITY_H

#include <string>
#include "Effects.h"
#include "Squares.h"
#include "Characters.h"
#include "Utils.h"

class Capacity
{
	private:
		std::string nameCapa;
		int damage;
		int heal;
		int percentage;
		bool isPassive;

		Effect* eft;

		TupleTC typeC;

	public:
		Square* launcher;
		Square* tabTargets;
		int nbTargets;

	public:
		Capacity();
		Capacity(std::string line);
		~Capacity();

		void addTarget(Square* s);
		void use();

		const std::string& getName() const;
		int getDamage() const;
		int getHeal() const;
		int getPercentage() const;
};

#endif