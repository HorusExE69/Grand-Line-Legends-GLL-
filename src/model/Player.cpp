#include "Player.h"
#include "Utils.h"
#include <iostream>
#include <fstream>
using namespace std;

const int CAPA_BANK = 67;

// Constructeur
Player::Player(string p)
{

	pseudo = p;
	lvl = 1;
	berries = 0;

	// Bank
	bankCapacity = CAPA_BANK;
	nbBank = 0;
	bank = new Character*[bankCapacity];

	// Unlocked
	ulkCapacity = bankCapacity;
	nbUnlock = 0;
	unlocked = new Character*[ulkCapacity];

	// Équipe
	teamCapacity = 1;
	teamSize = 0;
	team = new Character*[teamCapacity];
}



Player::Player(ifstream& file, string p)
{
	pseudo = p;
	lvl = 1;
	berries = 0;

	// Bank
	bankCapacity = CAPA_BANK;
	nbBank = 0;
	bank = new Character*[bankCapacity];

	string line;
	getline(file, line); // ignorer l'en-tête

	// Lire toutes les lignes du CSV
	while (getline(file, line))
	{
		if (line.empty()) continue;

		size_t pos = 0;

		// Ignorer l'ID
		pos = line.find(',');
		line = line.substr(pos + 1);

		// Name
		pos = line.find(',');
		string name = clean(line.substr(0, pos));
		line = line.substr(pos + 1);

		// Type
		pos = line.find(',');
		Type_ type = stringToType_(clean(line.substr(0, pos)));
		line = line.substr(pos + 1);

		// Rarity
		pos = line.find(',');
		Rarity rar = stringToRarity(clean(line.substr(0, pos)));
		line = line.substr(pos + 1);

		// PV
		pos = line.find(',');
		int pv = stoi(clean(line.substr(0, pos)));
		line = line.substr(pos + 1);

		// Speed
		pos = line.find(',');
		int speed = stoi(clean(line.substr(0, pos)));
		line = line.substr(pos + 1);

		// HakiR
		pos = line.find(',');
		int hakiR = stoi(clean(line.substr(0, pos)));
		line = line.substr(pos + 1);

		// HakiA
		pos = line.find(',');
		int hakiA = stoi(clean(line.substr(0, pos)));
		line = line.substr(pos + 1);

		// HakiO (dernier champ)
		int hakiO = stoi(clean(line));

		// Créer le personnage et l'ajouter à la bank
		Character* c = new Character(name, type, rar, pv, speed, hakiR, hakiO, hakiA);
		addToBank(c);
	}

	// Unlocked
	ulkCapacity = bankCapacity;
	nbUnlock = 0;
	unlocked = new Character*[ulkCapacity];

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
	delete[] unlocked;
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
		cout << "  " << RarityTostring(c->rarC()) << " - " << c->getName() << " - PV: " << c->getPV() << endl;
	}
}

// Unlocked

void Player::addToUnlocked(Character* c)
{
	if (c == nullptr) return;

	if (nbUnlock >= ulkCapacity)
	{
		int newCap = ulkCapacity * 2;
		Character** newUlk = new Character*[newCap];
		for (int i = 0; i < nbUnlock; i++) newUlk[i] = unlocked[i];
		delete[] unlocked;
		unlocked = newUlk;
		ulkCapacity = newCap;
	}

	unlocked[nbUnlock++] = c;
}


Character* Player::getUnlockCharacter(int index) const
{
	if (index >= 0 && index < nbUnlock) return unlocked[index];
	return nullptr;
}

int Player::getNbUnlock() const { return nbUnlock; }

void Player::showUnlocked() const
{
	cout << "Unlocked (" << nbUnlock << " persos):" << endl;
	for (int i = 0; i < nbUnlock; i++)
	{
		Character* c = unlocked[i];
		cout << "  " << Type_Tostring(c->typeC()) << " - " << c->getName() << " - PV: " << c->getPV() << endl;
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
	cout << "Equipe active (" << teamSize << "/" << teamCapacity << "):" << endl;
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