#ifndef SHOP_H
#define SHOP_H

#include "Coffre.h"

class Shop 
{
    private:
        Coffre* coffre;
        int nb_max_c;
    public:
        Shop(int max_c = 60);
        ~Shop();
        void open();
        void exit();
        void openCoffre(int index);
};

#endif