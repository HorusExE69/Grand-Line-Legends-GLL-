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
		int bankMax;

		Character** unlocked;
		int nbUnlock;
		int ulkMax;

		Character** team;
		int teamSize;
		int teamMax;

	public:
		Player(std::string p="Player");
		Player(std::ifstream& file, std::string path, std::string p="Player");
		~Player();

		void changePseudo(std::string p);

		void addToBank(Character* c);
		Character* getBankCharacter(int index) const;
		int getNbBank() const;
		int getBankMax() const;

		void addToUnlocked(Character* c);
		Character* getUnlockCharacter(int index) const;
		int getNbUnlock() const;
		int getUlkMax() const;
		void unlockAll();

		bool addToTeam(Character* c);
		void removeFromTeam(int index);
		int getTeamMax() const;
		void addTeamSize(int nb);
		int getTeamSize() const;
		Character* getTeamCharacter(int index) const;
		void randomTeam();

		std::string getPseudo() const;
		int getLvl() const;
		int getBerries() const;

		void setLvl(int l);
		void addLvl(int l);
		void setBerries(int b);
		void addBerries(int b);
};

#endif