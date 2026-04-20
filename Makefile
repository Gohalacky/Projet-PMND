CC      = gcc
CFLAGS  = -Wall -Wextra -Werror -std=c11 -g -I. -IInterface -IModele -IControlleur $(shell sdl2-config --cflags)
LDFLAGS = -lcmocka
LIBS    = $(shell sdl2-config --libs) -lSDL2_image -lSDL2_ttf

IFACE_DIR   = Interface
UI_DIR      = Interface/ui
MOD_DIR     = Modele
CTRL_DIR    = Controlleur

SRC_MOD = $(MOD_DIR)/alea.c        \
          $(MOD_DIR)/cartes.c       \
          $(MOD_DIR)/eco.c          \
          $(MOD_DIR)/evenements.c   \
          $(MOD_DIR)/hypo.c         \
          $(MOD_DIR)/player.c       \
          $(MOD_DIR)/property.c     \
          $(MOD_DIR)/tool.c         \
          $(MOD_DIR)/trade.c		\
		  $(MOD_DIR)/prison.c		\
          $(MOD_DIR)/voe.c		\

SRC_CTRL = $(CTRL_DIR)/game_engine.c \
           $(CTRL_DIR)/plateau.c

SRC_IFACE = $(IFACE_DIR)/main_interface.c \
            $(IFACE_DIR)/view.c           \
            $(IFACE_DIR)/building.c       \
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
OBJ_COMMON = $(OBJ_MOD)

ALL_OBJ   = $(OBJ_MOD) $(OBJ_CTRL) $(OBJ_IFACE)

TARGET      = insapoly
TEST_EXEC   = unit_tests
TEST_CARTES = test_cartes
CONTROLLEUR = controlleur

.DEFAULT_GOAL := all


all: $(TARGET)

$(TARGET): $(ALL_OBJ)
	$(CC) $^ -o $@ $(LIBS)
	@echo "✔  Compilation réussie → ./$(TARGET)"

test: $(MOD_DIR)/test.o $(OBJ_COMMON)
	$(CC) $(CFLAGS) -Wl,--wrap=rand $^ -o $(TEST_EXEC) $(LDFLAGS) $(LIBS)

test_cartes: $(MOD_DIR)/main_test_cartes.o $(OBJ_COMMON)
	$(CC) $(CFLAGS) $^ -o $(TEST_CARTES) $(LDFLAGS) $(LIBS)

controlleur: $(CTRL_DIR)/main.o $(OBJ_CTRL) $(OBJ_COMMON)
	$(CC) $(CFLAGS) $^ -o $(CONTROLLEUR) $(LDFLAGS) $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	@echo "▶  Lancement depuis $(IFACE_DIR)/ …"
	cd $(IFACE_DIR) && ../$(TARGET)

run_unit_tests: test
	cd $(MOD_DIR) && ../$(TEST_EXEC)

run_test: test_cartes
	cd $(MOD_DIR) && ../$(TEST_CARTES)

run_controlleur: controlleur
	./$(CONTROLLEUR)

doc:
	doxygen Doxyfile
	@echo "Documentation générée dans docs/html/"
	xdg-open docs/html/index.html

clean:
	rm -f $(ALL_OBJ) $(MOD_DIR)/test.o $(MOD_DIR)/main_test_cartes.o $(CTRL_DIR)/main.o
	rm -f $(TARGET) $(TEST_EXEC) $(TEST_CARTES) $(CONTROLLEUR)
	@echo "✔  Nettoyage terminé"

mrproper: clean
	rm -rf docs/

.PHONY: all run run_unit_tests run_test run_controlleur valgrind doc clean mrproper