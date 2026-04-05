CC      = gcc
CFLAGS  = -Wall -Wextra -std=c11 -g \
           -I. -IInterface -IModele -IControlleur \
           $(shell sdl2-config --cflags)
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
             $(IFACE_DIR)/events.c         \
             $(IFACE_DIR)/assets.c         \
             $(IFACE_DIR)/bridge.c         \
             $(IFACE_DIR)/adminWindow.c    \
             $(IFACE_DIR)/anim.c           \
             $(UI_DIR)/button.c            \
             $(UI_DIR)/text.c              \
             $(UI_DIR)/textbox.c           \
             $(UI_DIR)/counter.c           \
             $(UI_DIR)/playercard.c

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


%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

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