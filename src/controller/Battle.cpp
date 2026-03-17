#include "Battle.h"
#include <iostream>

using namespace std;

Battle::Battle(Player* p) : player(p), map(nullptr), turn(0) 
{
    cout << "Battle created!" << endl;
}

Battle::~Battle() 
{
    cout << "Battle destroyed!" << endl;
}

void Battle::start() 
{
    cout << "Battle started!" << endl;
}

void Battle::end() 
{
    cout << "Battle ended!" << endl;
}