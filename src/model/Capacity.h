#ifndef CAPACITY_H
#define CAPACITY_H

#include <string>
#include "Effects.h"
#include "Squares.h"

enum class TypeC
{
	heal,
	proj,
	fire,
	light,
	sup,
	blade,
	punch,
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

		Effect* eft;

		TupleTC typeC;

		Square* launcher;
		Square* tabTarget;
	public:
		Capacity(void);
		~Capacity(void);
};

#endif