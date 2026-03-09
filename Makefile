# ----------------------------
# Makefile pour Grand Line Legends
# ----------------------------

# Nom de l'exécutable
NAME = GLL

# Dossiers
SRC_DIR = src/model
OBJ_DIR = obj
BIN_DIR = bin

# Fichiers sources et objets
SRCS := $(wildcard $(SRC_DIR)/*.cpp)
OBJS := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRCS))

# Compilateur et flags
CXX = g++
CXXFLAGS = -g -Wall

# Règles

all: $(BIN_DIR)/$(NAME)

# Compilation de l'exécutable
$(BIN_DIR)/$(NAME): $(OBJS) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# Compilation des objets
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Création des dossiers obj et bin si manquants
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# Nettoyage

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(BIN_DIR)/$(NAME)

re: fclean all

.PHONY: all clean fclean re