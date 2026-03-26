CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c11 -g -I. -IInterface -IModele
LDFLAGS = -lcmocka
LIBS = $(shell sdl2-config --libs) -lSDL2_image -lSDL2_ttf

INTERFACE_DIR = Interface
MODELE_DIR    = Modele
CONTROLLEUR_DIR = Controlleur

SRC_MODELE_ONLY = $(MODELE_DIR)/alea.c \
                  $(MODELE_DIR)/cartes.c \
                  $(MODELE_DIR)/eco.c \
                  $(MODELE_DIR)/evenements.c \
                  $(MODELE_DIR)/player.c \
                  $(MODELE_DIR)/property.c \
                  $(MODELE_DIR)/tool.c \

OBJ_MODELE_ONLY = $(SRC_MODELE_ONLY:.c=.o)

# MOTEUR DE JEU (CONTROLLEUR) 
SRC_ENGINE = $(CONTROLLEUR_DIR)/game_engine.c $(CONTROLLEUR_DIR)/plateau.c
OBJ_ENGINE = $(SRC_ENGINE:.c=.o)

SRC_COMMON = $(SRC_MODELE_ONLY) $(INTERFACE_DIR)/view.c
OBJ_COMMON = $(SRC_COMMON:.c=.o)

EXEC        = insapoly
TEST_EXEC   = unit_tests
TEST_CARTES = test_cartes
VIEW        = interface
CONTROLLEUR = controlleur

all: $(EXEC)

$(EXEC): $(MODELE_DIR)/main.o $(OBJ_COMMON)
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

interface: $(INTERFACE_DIR)/main_view.o $(OBJ_COMMON)
	$(CC) $(CFLAGS) $^ -o $(VIEW) $(LIBS)

test: $(MODELE_DIR)/test.o $(OBJ_COMMON)
	$(CC) $(CFLAGS) $^ -o $(TEST_EXEC) $(LDFLAGS) $(LIBS)

test_cartes: $(MODELE_DIR)/main_test_cartes.o $(OBJ_COMMON)
	$(CC) $(CFLAGS) $^ -o $(TEST_CARTES) $(LDFLAGS) $(LIBS)

controlleur: $(CONTROLLEUR_DIR)/main.o $(OBJ_ENGINE) $(OBJ_COMMON)
	$(CC) $(CFLAGS) $^ -o $(CONTROLLEUR) $(LDFLAGS) $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

doc:
	doxygen Doxyfile
	@echo "Documentation générée dans le dossier docs/"
	xdg-open docs/html/index.html

clean:
	rm -f $(INTERFACE_DIR)/*.o $(MODELE_DIR)/*.o $(CONTROLLEUR_DIR)/*.o *.o
	rm -f $(EXEC) $(TEST_EXEC) $(TEST_CARTES) $(VIEW) $(CONTROLLEUR)
	rm -rf docs/

run_interface: interface
	cd $(INTERFACE_DIR) && ../$(VIEW)

run_test: test_cartes
	cd $(MODELE_DIR) && ../$(TEST_CARTES)

run_unit_tests: test
	cd $(MODELE_DIR) && ../$(TEST_EXEC)


run_controlleur : $(CONTROLLEUR)
	./$(CONTROLLEUR)