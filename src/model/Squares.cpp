#include "Squares.h"
#include "Characters.h"

Square::Square(int posX, int posY, Character* C)
{
	x=posX;
	y=posY;
	fill=0;
	inmate=C;
	if(inmate != nullptr)
	{
		fill=1;
	}
}

Square::~Square(void)
{
	x=0;
	y=0;
	fill=0;
}

void Square::hold(Character* C)
{
	fill = 1;
	inmate = C;
}

void Square::unhold(void)
{
	fill = 0;
	inmate = nullptr;
}

void Square::swap(Square* S)
{
	Character* temp;
	temp = S->inmate;
	S->hold(inmate);
	inmate = temp;
}

bool Square::isEmpty(void)
{
	return fill == 0;
}