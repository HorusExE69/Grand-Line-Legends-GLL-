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
	Player player(file, "./data/capacities/", "Horus");

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

	cout << "\n";

	string perso;

	cout << "Choisissez un personnage de la bank" << endl;
	cin >> perso;
	Character* c = player.getBankCharacter(stoi(perso));

	cout << "\n\n" << "Test capacites :\n" << endl;
	c->showCapa();

	cout << "\n\n" << "Test chooseCapa :\n" << endl;


	for(int i = 0; i < 10; i++)
	{
		Capacity* cap = c->chooseCapa();

		if(cap)
			cout << cap->getName() << endl;
	}

	Capacity* cap = c->chooseCapa();

	cout << "\n\nCapacite choisie : " << cap->getName() << " de type " << EffectTostring(cap->getEffect()->getType()) << endl;

	Square s1(0,0,c);
	cout << "PV avant degats : " << c->getPV() << endl;

	// Effect* e = new Effect(EffectType::Damage, TargetType::Self, 500, 1, nullptr, nullptr);

	// c->applyEffect(e);

	cap->addTarget(&s1);
	cap->use();

	cout << "PV apres degats : " << c->getPV() << endl;

	return(0);
}