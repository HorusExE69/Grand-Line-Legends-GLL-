#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include "../model/Player.h"
#include "../model/Characters.h"

using namespace std;

int main() {
	// Nettoyage des sous-chaînes (espaces et \r)
	auto clean = [](string s) -> string {
		while (!s.empty() && (s.back() == '\r' || s.back() == ' ')) s.pop_back();
		while (!s.empty() && (s.front() == ' ')) s.erase(0,1);
		return s;
	};

	// Ouvrir le CSV depuis le dossier data (relatif à bin/)
	ifstream file("./data/Characters.csv");
	if (!file.is_open())
	{
		cout << "Erreur : impossible d'ouvrir ../data/Characters.csv" << endl;
		return 1;
	}

	// Créer le joueur
	Player player("Horus");

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
		player.addToBank(c);
	}

	file.close();

	cout << "Total de personnages chargés dans la bank : " << player.getNbBank() << endl;

	cout << "\nÉquipe initiale :" << endl;
	player.addToTeam(player.getBankCharacter(0));
	player.showTeam();

	// Débloquer un slot d'équipe
	player.addTeamSize(4);
	player.addToTeam(player.getBankCharacter(rand()%67));
	player.addToTeam(player.getBankCharacter(rand()%67));
	player.addToTeam(player.getBankCharacter(rand()%67));

	cout << "\nÉquipe après déblocage d'un slot :" << endl;
	player.showTeam();

	return(0);
}