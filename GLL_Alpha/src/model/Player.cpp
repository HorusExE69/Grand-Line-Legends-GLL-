#include "Player.h"
#include "Utils.h"
#include <iostream>
#include <fstream>
using namespace std;

// Constructeur
Player::Player(string p)
{

	pseudo = p;
	lvl = 1;
	berries = 50;

	// Bank
	bankMax = 1;
	nbBank = 1;
	bank = new Character*[bankMax];

	// Unlocked
	ulkMax = bankMax;
	nbUnlock = 0;
	unlocked = new Character*[ulkMax];

	// Équipe
	teamMax = 1;
	teamSize = 0;
	team = new Character*[teamMax];
}



Player::Player(ifstream& file, string path, string p)
{
	pseudo = p;
	lvl = 1;
	berries = 50;

	// Bank
	bankMax = countLines(file);
	nbBank = 0;
	bank = new Character*[bankMax];

	string line;
	getline(file, line); // ignorer l'en-tête

	// Lire toutes les lignes du CSV
	while (getline(file, line))
	{
		if (line.empty()) continue;

		// Créer le personnage et l'ajouter à la bank
		Character* c = new Character(line, path);
		addToBank(c);
	}

	// Unlocked
	ulkMax = bankMax;
	nbUnlock = 0;
	unlocked = new Character*[ulkMax];

	// Équipe
	teamMax = 1;
	teamSize = 0;
	team = new Character*[teamMax];
}

// Destructeur
Player::~Player()
{
	for (int i = 0; i < nbBank; i++) 
	{
		delete bank[i];
	}
	delete[] bank;
	delete[] unlocked;
	delete[] team;
}

void Player::changePseudo(string p)
{
	pseudo = p;
}

// Bank
void Player::addToBank(Character* c)
{
	if (c == nullptr) return;

	if (nbBank >= bankMax)
	{
		int newCap = bankMax * 2;
		Character** newBank = new Character*[newCap];
		for (int i = 0; i < nbBank; i++) newBank[i] = bank[i];
		delete[] bank;
		bank = newBank;
		bankMax = newCap;
	}

	bank[nbBank++] = c;
}

Character* Player::getBankCharacter(int index) const
{
	if (index >= 0 && index < nbBank) return bank[index];
	return nullptr;
}

int Player::getNbBank() const { return nbBank; }

// Unlocked

void Player::addToUnlocked(Character* c)
{
	if (c == nullptr) return;

	if (nbUnlock >= ulkMax) return;

	unlocked[nbUnlock++] = c;
}

void Player::unlockAll()
{
	for (int i = 0; i < nbBank; i++)
	{
		addToUnlocked(bank[i]);
	}
}

Character* Player::getUnlockCharacter(int index) const
{
	if (index >= 0 && index < nbUnlock) return unlocked[index];
	return nullptr;
}

int Player::getNbUnlock() const { return nbUnlock; }


// Équipe
bool Player::addToTeam(Character* c)
{
	if (c == nullptr) return false;
	if (teamSize >= teamMax) return false;

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

void Player::addTeamSize(int nb)
{
	teamMax += nb;
	Character** newTeam = new Character*[teamMax];
	for (int i = 0; i < teamSize; i++) newTeam[i] = team[i];
	delete[] team;
	team = newTeam;
}

int Player::getTeamSize() const { return teamSize; }


Character* Player::getTeamCharacter(int index) const
{
	if (index >= 0 && index < teamSize) 
	{
		return team[index];
	}
	return nullptr;
}

void Player::randomTeam()
{
	teamSize = 0;

	for (int i = 0; i < teamMax; i++)
	{
		int idx = rand() % nbUnlock;
		addToTeam(unlocked[idx]);
	}
}

// Accesseurs / Setters
string Player::getPseudo() const { return pseudo; }
int Player::getLvl() const { return lvl; }
int Player::getBerries() const { return berries; }
int Player::getBankMax() const { return bankMax; }
int Player::getUlkMax() const { return ulkMax; }
int Player::getTeamMax() const { return teamMax; }

void Player::setLvl(int l) { lvl = l; }
void Player::addLvl(int l) { lvl += l; }
void Player::setBerries(int b) { berries = b; }
void Player::addBerries(int b) { berries += b; }