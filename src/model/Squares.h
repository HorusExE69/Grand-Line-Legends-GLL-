#ifndef SQUARE_H
#define SQUARE_H

class Character;

class Square
{
	public:
		int x;
		int y;

		bool fill;

		Character* inmate;

		Square(void);
		Square(int posX=0, int posY=0, Character* C=nullptr);
		~Square(void);
		void hold(Character* C);
		void unhold(void);
		void swap(Square* S);
};

#endif