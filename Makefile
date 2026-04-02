CXX = g++
CXXFLAGS = -Wall -g -fPIE
LDFLAGS = -pie -lSDL2 -lSDL2_ttf

# Dossiers
SRC_DIRS = src/model src/controller src/view
OBJ_DIR = obj
BIN_DIR = bin

# Tous les fichiers sources
SRCS := $(shell find $(SRC_DIRS) -name "*.cpp")

# Fichiers objets correspondants
OBJS := $(patsubst src/%.cpp, $(OBJ_DIR)/%.o, $(SRCS))

# Cible par défaut
all: $(BIN_DIR)/GLL

# Compilation des fichiers objets
$(OBJ_DIR)/%.o: src/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Linkage de l'exécutable
$(BIN_DIR)/GLL: $(OBJS)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $@ $(LDFLAGS)

# Nettoyage
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)/GLL

.PHONY: all clean