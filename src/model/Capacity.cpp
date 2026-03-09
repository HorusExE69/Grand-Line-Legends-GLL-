#include "Capacity.h"

using namespace std;

Capacity::Capacity(void)
{
	nameCapa = "";
	damage = 0;
	heal = 0;
	loadTime = 0;

	tabEft = nullptr;

	typeC.t1 = TypeC::other;
	typeC.t2 = TypeC::other;

	launcher = nullptr;
	tabTarget = nullptr;
}

Capacity::~Capacity(void)
{
	if(tabEft != nullptr)
	{
		delete[] tabEft;
	}
	if(launcher != nullptr)
	{
		delete launcher;
	}
	if(tabTarget != nullptr)
	{
		delete[] tabTarget;
	}
}