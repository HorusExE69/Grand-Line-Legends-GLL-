#ifndef CAPACITY_H
#define CAPACITY_H

#include <string>
#include "Effects.h"
#include "Squares.h"
#include "Characters.h"

enum class TypeC
{
	heal,
	proj,
	fire,
	light,
	sup,
	blade,
	punch,
	air,
	ice,
	kick,
	electricity,
	water,
	magic,
	self_buff,
	other
};

struct TupleTC
{
	TypeC t1;
	TypeC t2;
};

class Capacity
{
	private:
		std::string nameCapa;
		int damage;
		int heal;
		int loadTime;

		Effect** effects;
		int nbEffects;

		TupleTC typeC;

	public:
		Square* launcher;
		Square** targets;
		int nbTargets;

	public:
		Capacity(const std::string& name="", int dmg=0, int hl=0, int lt=0);
		~Capacity();

		void addEffect(Effect* e);
		void addTarget(Square* s);
		void use();

		const std::string& getName() const;
		int getDamage() const;
		int getHeal() const;
};

#endif