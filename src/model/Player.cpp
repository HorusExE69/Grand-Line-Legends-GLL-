#include "Player.h"
#include <iostream>
using namespace std;

// Constructeur
Player::Player(string p)
{
	pseudo = p;
	lvl = 1;
	berries = 0;

	// Bank
	bankCapacity = 8;
	nbBank = 0;
	bank = new Character*[bankCapacity];

	// Équipe
	teamCapacity = 1;
	teamSize = 0;
	team = new Character*[teamCapacity];
}

// Destructeur
Player::~Player()
{
	for (int i = 0; i < nbBank; i++) 
	{
		delete bank[i];
	}
	delete[] bank;

	delete[] team;
}

// Bank
void Player::addToBank(Character* c)
{
	if (c == nullptr) return;

	if (nbBank >= bankCapacity)
	{
		int newCap = bankCapacity * 2;
		Character** newBank = new Character*[newCap];
		for (int i = 0; i < nbBank; i++) newBank[i] = bank[i];
		delete[] bank;
		bank = newBank;
		bankCapacity = newCap;
	}

	bank[nbBank++] = c;
}

Character* Player::getBankCharacter(int index) const
{
	if (index >= 0 && index < nbBank) return bank[index];
	return nullptr;
}

int Player::getNbBank() const { return nbBank; }

void Player::showBank() const
{
	cout << "Bank (" << nbBank << " persos):" << endl;
	for (int i = 0; i < nbBank; i++)
	{
		Character* c = bank[i];
		cout << "  " << c->getName() << " - PV: " << c->getPV() << endl;
	}
}

// Équipe
bool Player::addToTeam(Character* c)
{
	if (c == nullptr) return false;
	if (teamSize >= teamCapacity) return false;

	team[teamSize++] = c;
	return true;
}

void Player::removeFromTeam(int index)
{
	if (index < 0 || index >= teamSize) return;

	for (int i = index; i < teamSize - 1; i++)
	{
		team[i] = team[i + 1];
	}
	teamSize--;
}

void Player::showTeam() const
{
	cout << "Équipe active (" << teamSize << "/" << teamCapacity << "):" << endl;
	for (int i = 0; i < teamSize; i++)
	{
		Character* c = team[i];
		cout << "  " << c->getName() << " - PV: " << c->getPV() << endl;
	}
}

// Augmenter taille équipe
void Player::addTeamSize(int nb)
{
	teamCapacity += nb;
	Character** newTeam = new Character*[teamCapacity];
	for (int i = 0; i < teamSize; i++) newTeam[i] = team[i];
	delete[] team;
	team = newTeam;
}

// Accesseurs / Setters
string Player::getPseudo() const { return pseudo; }
int Player::getLvl() const { return lvl; }
int Player::getBerries() const { return berries; }

void Player::setLvl(int l) { lvl = l; }
void Player::addLvl(int l) { lvl += l; }
void Player::setBerries(int b) { berries = b; }
void Player::addBerries(int b) { berries += b; }