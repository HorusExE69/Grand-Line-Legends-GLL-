#ifndef PLAYER_H
#define PLAYER_H

#include <string>
#include "Characters.h"

class Player
{
private:
	std::string pseudo;
	int lvl;
	int berries;

	Character** bank;
	int nbBank;
	int bankCapacity;

	Character** team;
	int teamSize;
	int teamCapacity;

public:
	Player(std::string p="Player");
	~Player();
	void addToBank(Character* c);
	Character* getBankCharacter(int index) const;
	int getNbBank() const;
	bool addToTeam(Character* c);
	void removeFromTeam(int index);
	void showTeam() const;
	void showBank() const;
	void addTeamSize(int nb);
	std::string getPseudo() const;
	int getLvl() const;
	int getBerries() const;
	void setLvl(int l);
	void addLvl(int l);
	void setBerries(int b);
	void addBerries(int b);
};

#endif