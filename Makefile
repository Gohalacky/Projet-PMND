<<<<<<< HEAD
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
				  $(MODELE_DIR)/hypo.c \
				  $(MODELE_DIR)/trade.c \

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
=======
CC      = gcc
CFLAGS  = -Wall -Wextra -std=c11 -g -I. -IInterface -IModele -IControlleur $(shell sdl2-config --cflags)
LIBS    = $(shell sdl2-config --libs) -lSDL2_image -lSDL2_ttf

IFACE_DIR = Interface
UI_DIR    = Interface/ui
MOD_DIR   = Modele
CTRL_DIR  = Controlleur

SRC_MOD = $(MOD_DIR)/alea.c      \
           $(MOD_DIR)/cartes.c    \
           $(MOD_DIR)/eco.c       \
           $(MOD_DIR)/evenements.c \
           $(MOD_DIR)/hypo.c      \
           $(MOD_DIR)/player.c    \
           $(MOD_DIR)/property.c  \
           $(MOD_DIR)/tool.c      \
           $(MOD_DIR)/trade.c

SRC_CTRL = $(CTRL_DIR)/game_engine.c \
            $(CTRL_DIR)/plateau.c

SRC_IFACE = $(IFACE_DIR)/main_interface.c  \
             $(IFACE_DIR)/view.c           \
             $(IFACE_DIR)/building.c      \
             $(IFACE_DIR)/events.c         \
             $(IFACE_DIR)/assets.c         \
             $(IFACE_DIR)/bridge.c         \
             $(IFACE_DIR)/adminWindow.c    \
             $(IFACE_DIR)/anim.c           \
             $(IFACE_DIR)/propwindow.c     \
             $(UI_DIR)/button.c            \
             $(UI_DIR)/text.c              \
             $(UI_DIR)/textbox.c           \
             $(UI_DIR)/counter.c           \
             $(UI_DIR)/playercard.c        \
             $(UI_DIR)/checkbox.c

OBJ_MOD   = $(SRC_MOD:.c=.o)
OBJ_CTRL  = $(SRC_CTRL:.c=.o)
OBJ_IFACE = $(SRC_IFACE:.c=.o)

ALL_OBJ   = $(OBJ_MOD) $(OBJ_CTRL) $(OBJ_IFACE)

TARGET = insapoly

.DEFAULT_GOAL := all

all: $(TARGET)

$(TARGET): $(ALL_OBJ)
	$(CC) $^ -o $@ $(LIBS)
	@echo "✔  Compilation réussie → ./$(TARGET)"

valgrind: $(TARGET)
	@echo "🔍 Lancement de Valgrind (analyse de mémoire)..."
	cd $(IFACE_DIR) && valgrind --leak-check=full \
		--show-leak-kinds=all \
		--track-origins=yes \
		--log-file=../valgrind_report.txt \
		../$(TARGET)
	@echo "📑 Analyse terminée. Consultez le fichier valgrind_report.txt"


>>>>>>> View_interface

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

<<<<<<< HEAD
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
=======
run: $(TARGET)
	@echo "▶  Lancement depuis $(IFACE_DIR)/ …"
	cd $(IFACE_DIR) && ../$(TARGET)

doc:
	doxygen Doxyfile
	@echo "Documentation générée dans docs/html/"

clean:
	rm -f $(ALL_OBJ) $(TARGET)
	@echo "✔  Nettoyage terminé"

mrproper: clean
	rm -rf docs/

.PHONY: all run doc clean mrproper
>>>>>>> View_interface
