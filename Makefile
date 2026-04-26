# Makefile — Grand Line Legends
#
# Cibles disponibles :
#   make          : version graphique SDL2  (bin/GLL)
#   make assets   : génère les images BMP dans assets/ (à faire 1 fois)
#   make text     : version textuelle        (bin/GLL_text)
#   make tests    : compile et exécute tous les tests de régression
#   make docu     : génère la documentation Doxygen    (doc/html/)
#   make clean    : supprime les objets, exécutables et doc générés
#
# Dépendances SDL2 (version graphique uniquement) :
#   Linux  : sudo apt install libsdl2-dev libsdl2-ttf-dev libsdl2-image-dev
#   macOS  : brew install sdl2 sdl2_ttf sdl2_image
#   Windows: cf. README

.DEFAULT_GOAL := all

# Auto-installation des dépendances SDL2
install-deps:
	@echo "Installation des dependances SDL2"
	@if command -v apt-get >/dev/null 2>&1; then \
	    sudo apt-get install -y libsdl2-dev libsdl2-ttf-dev libsdl2-image-dev; \
	elif command -v dnf >/dev/null 2>&1; then \
	    sudo dnf install -y SDL2-devel SDL2_ttf-devel SDL2_image-devel; \
	elif command -v pacman >/dev/null 2>&1; then \
	    sudo pacman -S --noconfirm sdl2 sdl2_ttf sdl2_image; \
	elif command -v brew >/dev/null 2>&1; then \
	    brew install sdl2 sdl2_ttf sdl2_image; \
	else \
	    echo "Gestionnaire de paquets inconnu. Installez manuellement :"; \
	    echo "  Linux  : sudo apt install libsdl2-dev libsdl2-ttf-dev libsdl2-image-dev"; \
	    echo "  macOS  : brew install sdl2 sdl2_ttf sdl2_image"; \
	fi

# Vérifie la présence de SDL2 et installe si absent
.check-sdl2:
	@pkg-config --exists sdl2 2>/dev/null || (echo "SDL2 absent — installation automatique..." && $(MAKE) install-deps)

CXX      = g++
CXXFLAGS = -O1 -fPIE -std=c++11

# Compilation en parallèle (tous les cœurs disponibles)
JOBS     ?= $(shell nproc 2>/dev/null || echo 4)
MAKEFLAGS += -j$(JOBS)

# Détection changement de plateforme (évite les .o Windows sur Linux)
PLATFORM_STAMP = obj/.platform
CURRENT_PLATFORM := $(shell uname -s)-$(shell uname -m)
LAST_PLATFORM    := $(shell cat $(PLATFORM_STAMP) 2>/dev/null)
ifneq ($(CURRENT_PLATFORM),$(LAST_PLATFORM))
    $(shell rm -rf obj/*)
    $(shell mkdir -p obj)
    $(shell echo "$(CURRENT_PLATFORM)" > $(PLATFORM_STAMP))
endif

# Détection SDL2
# Essayer sdl2-config (Linux/macOS Homebrew), puis pkg-config
SDL2_CONFIG := $(shell command -v sdl2-config 2>/dev/null)
ifdef SDL2_CONFIG
    SDL_FLAGS = $(shell sdl2-config --cflags)
    SDL_LIBS  = $(shell sdl2-config --libs) -lSDL2_ttf -lSDL2_image
else
    SDL_FLAGS = $(shell pkg-config --cflags sdl2 2>/dev/null)
    SDL_LIBS  = $(shell pkg-config --libs   sdl2 2>/dev/null) \
                $(shell pkg-config --libs   SDL2_ttf   2>/dev/null) \
                $(shell pkg-config --libs   SDL2_image 2>/dev/null)
endif

# Dossiers
SRC_DIR  = src
OBJ_DIR  = obj
BIN_DIR  = bin
TEST_DIR = tests

# Sources

MODEL_SRC = \
	$(SRC_DIR)/model/Utils.cpp \
	$(SRC_DIR)/model/Effects.cpp \
	$(SRC_DIR)/model/Squares.cpp \
	$(SRC_DIR)/model/Capacity.cpp \
	$(SRC_DIR)/model/Characters.cpp \
	$(SRC_DIR)/model/BattleMap.cpp \
	$(SRC_DIR)/model/Reward.cpp \
	$(SRC_DIR)/model/Episode.cpp \
	$(SRC_DIR)/model/Arc.cpp \
	$(SRC_DIR)/model/Campaign.cpp \
	$(SRC_DIR)/model/Player.cpp \
	$(SRC_DIR)/model/Family.cpp \
	$(SRC_DIR)/model/Settings.cpp \
	$(SRC_DIR)/model/Coffre.cpp \
	$(SRC_DIR)/model/Shop.cpp \
	$(SRC_DIR)/model/SaveData.cpp

CTRL_SRC = \
	$(SRC_DIR)/controller/Battle.cpp \
	$(SRC_DIR)/controller/Game.cpp

VIEW_TEXT_SRC = \
	$(SRC_DIR)/view/winTxt.cpp \
	$(SRC_DIR)/view/ViewText.cpp

VIEW_GRAPH_SRC = $(SRC_DIR)/view/ViewGraph.cpp

# Objets

MODEL_OBJ      = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(MODEL_SRC))
CTRL_OBJ       = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(CTRL_SRC))
VIEW_TEXT_OBJ  = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(VIEW_TEXT_SRC))
VIEW_GRAPH_OBJ = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(VIEW_GRAPH_SRC))
MAIN_SDL_OBJ   = $(OBJ_DIR)/controller/main.o
MAIN_TEXT_OBJ  = $(OBJ_DIR)/controller/mainText.o
COMMON_OBJ     = $(MODEL_OBJ) $(CTRL_OBJ) $(VIEW_TEXT_OBJ)

# Règles de compilation

$(OBJ_DIR)/model/%.o: $(SRC_DIR)/model/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/controller/%.o: $(SRC_DIR)/controller/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/view/winTxt.o: $(SRC_DIR)/view/winTxt.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/view/ViewText.o: $(SRC_DIR)/view/ViewText.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/view/ViewGraph.o: $(SRC_DIR)/view/ViewGraph.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(SDL_FLAGS) -c $< -o $@

$(OBJ_DIR)/tests/%.o: $(TEST_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Génération des assets (images BMP)

GEN_ASSETS_SRC = tools/gen_assets.cpp
GEN_ASSETS_BIN = tools/gen_assets

assets: $(GEN_ASSETS_BIN)
	@mkdir -p assets
	./$(GEN_ASSETS_BIN)

$(GEN_ASSETS_BIN): $(GEN_ASSETS_SRC)
	$(CXX) $(CXXFLAGS) $< -o $@ -lm
	@echo ">>> Générateur : ./tools/gen_assets"

# Cible SDL2 (version graphique)

all: .check-sdl2 $(BIN_DIR)/GLL

# Cible debug (avec symboles gdb)
debug: CXXFLAGS = -g -Wall -fPIE -std=c++11
debug: $(BIN_DIR)/GLL

$(BIN_DIR)/GLL: $(COMMON_OBJ) $(VIEW_GRAPH_OBJ) $(MAIN_SDL_OBJ)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(SDL_LIBS)
	@echo ">>> Binaire graphique : ./bin/GLL"

# Cible textuelle

text: $(BIN_DIR)/GLL_text

$(BIN_DIR)/GLL_text: $(COMMON_OBJ) $(MAIN_TEXT_OBJ)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@
	@echo ">>> Binaire texte : ./bin/GLL_text"

# Tests de régression

tests: test_char test_player test_battle test_campaign

test_char: $(COMMON_OBJ) $(OBJ_DIR)/tests/testCharacter.o
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $(BIN_DIR)/testCharacter
	./$(BIN_DIR)/testCharacter

test_player: $(COMMON_OBJ) $(OBJ_DIR)/tests/testPlayer.o
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $(BIN_DIR)/testPlayer
	./$(BIN_DIR)/testPlayer

test_battle: $(COMMON_OBJ) $(OBJ_DIR)/tests/testBattle.o
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $(BIN_DIR)/testBattle
	./$(BIN_DIR)/testBattle

test_campaign: $(COMMON_OBJ) $(OBJ_DIR)/tests/testCampaign.o
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $(BIN_DIR)/testCampaign
	./$(BIN_DIR)/testCampaign

# Documentation Doxygen

docu:
	doxygen doc/doxyfile
	@echo ">>> Documentation : doc/html/index.html"

# Nettoyage

clean:
	rm -rf obj/* doc/html \
	       $(BIN_DIR)/GLL $(BIN_DIR)/GLL_text \
	       $(BIN_DIR)/testCharacter $(BIN_DIR)/testPlayer \
	       $(BIN_DIR)/testBattle    $(BIN_DIR)/testCampaign \
	       $(GEN_ASSETS_BIN)
	@rm -f $(PLATFORM_STAMP)

.PHONY: all debug assets text tests test_char test_player test_battle test_campaign docu clean install-deps .check-sdl2
