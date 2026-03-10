# ----------------------------
# Makefile pour Grand Line Legends
# ----------------------------

NAME = GLL

# Dossiers
SRC_DIRS = src/model src/controller
OBJ_DIR = obj
BIN_DIR = bin

# Sources
SRCS = $(wildcard src/model/*.cpp) $(wildcard src/controller/*.cpp)

# Objets
OBJS = $(patsubst src/%.cpp,$(OBJ_DIR)/%.o,$(SRCS))

# Compilateur
CXX = g++
CXXFLAGS = -Wall -g

all: $(BIN_DIR)/$(NAME)

$(BIN_DIR)/$(NAME): $(OBJS) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@

# Compilation des objets
$(OBJ_DIR)/%.o: src/%.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Création des dossiers
$(BIN_DIR):
	mkdir -p $(BIN_DIR)

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(BIN_DIR)/$(NAME)

re: fclean all

.PHONY: all clean fclean re