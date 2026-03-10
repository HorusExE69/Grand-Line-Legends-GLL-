#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include "../model/Player.h"
#include "../model/Characters.h"

using namespace std;

int main()
{
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

	cout << "Total de personnages charges dans la bank : " << player.getNbBank() << endl;

	player.showBank();

	cout << "\nEquipe initiale :" << endl;
	player.addToTeam(player.getBankCharacter(0));
	player.showTeam();

	// Débloquer un slot d'équipe
	player.addTeamSize(4);
	player.addToTeam(player.getBankCharacter(rand()%67));
	player.addToTeam(player.getBankCharacter(rand()%67));
	player.addToTeam(player.getBankCharacter(rand()%67));

	cout << "\nEquipe apres deblocage d'un slot :" << endl;
	player.showTeam();

	return(0);
}