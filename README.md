# Grand Line Legends

Jeu RPG tour par tour thème One Piece, inspiré de Souls et Raid Shadow Legends.

**Cours** : LIFAPCD — Université Claude Bernard Lyon 1  
**GitLab** : https://forge.univ-lyon1.fr/p2405025/grand-line-legends

## Auteurs

| Nom | Prénom | Numéro étudiant |
|-----|--------|-----------------|
| BRETAGNE | Hans | p2405025 |
| IRIS | Lisa | p2308359 |

---

## Compilation

### Prérequis

| Outil | Linux | macOS |
|-------|-------|-------|
| g++ | `sudo apt install g++` | Xcode Command Line Tools |
| SDL2 | `sudo apt install libsdl2-dev` | `brew install sdl2` |
| SDL2_ttf | `sudo apt install libsdl2-ttf-dev` | `brew install sdl2_ttf` |
| doxygen | `sudo apt install doxygen` | `brew install doxygen` |

### Commandes make

```
make              Version graphique SDL2    --> bin/GLL
make text         Version textuelle         --> bin/GLL_text
make tests        Tests de régression       --> bin/test*
make docu         Documentation Doxygen     --> doc/html/index.html
make clean        Supprime obj/ bin/ doc/html/
```

### Exécution

```bash
./bin/GLL           # Version graphique
./bin/GLL_text      # Version textuelle
```

### Tests individuels

```bash
./bin/testCharacter
./bin/testPlayer
./bin/testBattle
./bin/testCampaign
```

---

## Fonctionnalités

### Menu principal
- **[J] Jouer** : sélection/chargement de sauvegarde (3 emplacements), puis menu de jeu
- **[P] Paramètres** : fps (30/60), volume, luminosité
- **[Q] Quitter** : sauvegarde automatique

### Bank
- Liste des 67 personnages avec type, rareté, PV, niveau, statut débloqué
- Vue détaillée : stats complètes, Haki, capacités avec types et chances
- Déblocage : 100 fragments du personnage requis
- Amélioration : coût en berries + 100 fragments (croît avec le niveau et la rareté)

### Campagne (18 arcs)
Structure de chaque arc : sbires → mini-boss → boss final  
Récompenses : berries + fragments de personnage (avec probabilités de drop)

| Arc | Boss | Déblocage |
|-----|------|-----------|
| Romance Dawn | Morgan | Zoro 100% |
| Orange Town | Baggy | Nami 50% |
| Village Sirop | Kuro | Usopp 100% |
| Baratie | Don Krieg | Sanji 100% |
| Arlong Park | Arlong | Nami 50% |
| Logue Town | Smoker | Baggy 100% |
| Whiskey Peak | Mr5 & Miss Valentine | Vivi 100% |
| Little Garden | Mr3 & Miss GW | DoggyBroggy 50% |
| Drum Island | Wapol | Chopper 100% (Nami bannie) |
| Alabasta | Crocodile | Robin 100% |
| Skypea | Ener | — |
| Water Seven | Franky | Franky 50% |
| Enies Lobby | Rob Lucci | Franky 50% |
| Thriller Bark | Gecko Moria | Brook 100% |
| Amazon Lily | Boa Hancock | Boa Hancock 100% |
| Impel Down | Magellan | Bon Clay + multi 25% |
| Marine Ford | Akainu | Ace 50% + multi |
| Pantheon | Tous les Boss | arc final sans fin |

### Combat
- Grille 2×10 (joueur ligne 0, ennemi ligne 1)
- Ordre d'attaque par vitesse décroissante
- Capacités choisies aléatoirement selon leurs pourcentages
- Effets : Damage, Heal, Stun, Bleeding, Resist, Push, Pull, Swap
- Log des actions affiché à chaque tour
- Haki des Rois : bonus/malus selon la différence de niveau
- Haki de l'Armement : bonus de dégâts
- Haki de l'Observation : chance d'esquiver les attaques

### Familles (bonus d'équipe)

| Famille | Membres | Bonus |
|---------|---------|-------|
| Les Monkey D. | Garp, Luffy, Dragon | +25% PV |
| Trio de Frères | Sabo, Ace, Luffy | +25% Attaque |
| Révolutionnaires | Sabo, Dragon, Ivankov | +15% Défense |
| Empereurs | BarbeBlanche, BarbeNoire, Shanks, Kaido, BigMom | +20% PV |
| Amiraux | Akainu, Aokiji, Kizaru, Fujitora | +20% Défense |
| Mugiwara | Les 9 Chapeaux de Paille | +20% Attaque |
| Marines | Garp, Akainu, Aokiji, Kizaru, Smoker, Sengoku | +15% Défense |
| Équipage BB | BarbeBlanche, Marco, Ace | +30% Attaque |

### Shop
- Offre journalière, hebdomadaire, mensuelle (changent automatiquement)
- 5 offres permanentes : Coffres Rare / Super Rare / Épique / Légendaire / Mythique

### Système de progression
- **Berries** : monnaie gagnée en combat et au shop
- **XP** : gagnée en complétant des épisodes → niveau de compte 1 à 10  
  Seuil : niveau² × 600 (ex : niv.1→2 : 600 XP, niv.9→10 : 48 600 XP)
- **Slots d'équipe** : 1 au départ, +1 par niveau de compte (max 10)
- **Fragments** : 100 fragments par personnage pour le débloquer, puis améliorer son niveau

### Sauvegarde
Au lancement : choix nouvelle partie ou chargement (slots 1-3).  
Sauvegarde automatique après chaque victoire et à la fermeture.  
Fichiers : `data/save1.txt`, `data/save2.txt`, `data/save3.txt`

---

## Organisation de l'archive

```
grand-line-legends/
├── Makefile                  Compile les deux versions + tests + doc
├── README.md                 Ce fichier
├── doc/
│   ├── doxyfile              Configuration Doxygen
│   ├── html/                 Documentation générée (make docu)
│   └── uml/
│       ├── classes.pdf       Diagramme de classes UML
│       ├── mvc.pdf           Architecture MVC
│       ├── battle_sequence.pdf  Séquence combat
│       ├── gantt.pdf         Planning Gantt (avec répartition des tâches)
│       └── presentation.pdf  Présentation orale de soutenance
├── src/
│   ├── model/                Modèle : données du jeu (16 modules)
│   ├── controller/           Contrôleurs : logique de jeu
│   └── view/                 Vues : affichage (texte et graphique SDL2)
├── data/
│   ├── Characters.csv        67 personnages (id, nom, type, rareté, stats)
│   ├── capacities/           Un CSV par personnage (attaques et effets)
│   └── settings.txt          Paramètres (généré à l'exécution)
└── tests/
    ├── testCharacter.cpp     7 tests Character
    ├── testPlayer.cpp        11 tests Player
    ├── testBattle.cpp        5 tests Battle
    └── testCampaign.cpp      10 tests Campaign + SaveData
```

---

## Architecture MVC

```
main.cpp / mainText.cpp
        |
        v
    Game (contrôleur central — états, entrées, dispatch)
   +-------+------------+-------------+----------+
   |       |            |             |          |
Player  Campaign     Settings       Shop     Battle
   |       |                                    |
Bank    Arcs --> Episodes --> Rewards      BattleMap
   |
Characters --> Capacities --> Effects
```

La vue (`ViewText` / `ViewGraph`) ne modifie **jamais** le modèle directement.  
Elle émet des `Event` que `Game::update()` traite, puis relit l'état en lecture seule.

---

## Format des données

### `data/Characters.csv`
```
id,name,type,rarity,pv,speed,hakiR,hakiA,hakiO
1,Luffy,atk,c,4000,105,80,90,90
```
- **type** : `atk` / `def` / `sup` / `int` / `snp` / `mag`
- **rarity** : `c` / `r` / `sr` / `e` / `l` / `m`

### `data/capacities/<Nom>.csv`
```
id,character,name,type1,type2,damage,heal,targetType,targetCount,gearLevel,unlockLvl,effect,percentage
1,Ace,Fire Fist,fire,punch,2300,0,Enemy,1,3,15,Damage,40
```
- **targetType** : `Self` / `Ally` / `Enemy` / `AllAllies` / `AllEnemies` / `All`
- **effect** : `Damage` / `Heal` / `Buff` / `Debuff` / `Resist` / `Stun` / `Swap` / `Bleeding` / `Push` / `Pull`
- **percentage** : probabilité d'utilisation (100 = capacité passive)

---

## Règles de programmation

- Pas de variable globale
- Passage par `const Type&` en entrée, `Type&` en entrée/sortie
- `const` sur tous les accesseurs
- `assert()` pour valider les préconditions
- Gestion mémoire : chaque `new` a son `delete` correspondant
- Doxygen : tous les headers documentés (`/** @brief ... */`)
- Modularité : une classe = un module `.h`/`.cpp`
- Aucun `cout`/`cin` dans `ViewText` : `WinTXT` utilisé exclusivement
- `using namespace std;` dans les `.cpp` uniquement
