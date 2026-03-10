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
	Player player(file, "Horus");

	file.close();

	cout << "Total de personnages chargés dans la bank : " << player.getNbBank() << endl;

	player.showBank();

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