#include <iostream>
#include "Shop.h"

using namespace std;

Shop::Shop(int max_c)
{
    for (int i=0, i < max_c; i++)
    {
        coffre.push_back(Coffre(i));
    }
}

void Shop::open()
{
    cout << "Bienvenue dans le Shop Merry !" << endl;
    cout << "Coffres disponibles :" << endl;
    for (size_t i=0; i < coffre.size(); i++)
    {
        cout << i << ". Coffre ID " << coffre[i].getID() << endl;
    }
}

void Shop::close()
{
    cout << "Fermeture du Shop Merry" << endl;
}

void Shop::openCoffre(int index)
{
    if (index < 0 || index >= )
    {
        cout << "Coffre invalide" << endl;
        return;
    }
    cout << "Ouverture du coffre " << index << "..." <<endl;
    coffre.open(index);
}