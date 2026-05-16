CC      = gcc
CFLAGS  = -Wall -Wextra -g -Isrc/model -Isrc/interface -Isrc/controller $(shell sdl2-config --cflags)
LDFLAGS = -lcmocka
LIBS    = $(shell sdl2-config --libs) -lSDL2_image -lSDL2_ttf -lm -lcjson

SRC_DIR  = src
MOD_DIR  = src/Model
CTRL_DIR = src/Controller
IFACE_DIR = src/Interface
UI_DIR   = src/Interface/ui
BUILD_DIR = build
BIN_DIR   = bin

SRC_MOD = $(MOD_DIR)/cartes.c       \
          $(MOD_DIR)/evenements.c   \
          $(MOD_DIR)/hypo.c         \
          $(MOD_DIR)/player.c       \
          $(MOD_DIR)/property.c     \
          $(MOD_DIR)/tool.c         \
          $(MOD_DIR)/trade.c        \
          $(MOD_DIR)/prison.c

SRC_CTRL = $(CTRL_DIR)/game_engine.c \
           $(CTRL_DIR)/engine_logic.c \
           $(CTRL_DIR)/plateau.c

SRC_CTRL_CONSOLE = $(CTRL_DIR)/game_engine_console.c \
                   $(CTRL_DIR)/engine_logic.c         \
                   $(CTRL_DIR)/plateau.c

SRC_IFACE = $(IFACE_DIR)/main_interface.c \
            $(IFACE_DIR)/view.c           \
            $(IFACE_DIR)/building.c       \
            $(IFACE_DIR)/events.c         \
            $(IFACE_DIR)/assets.c         \
            $(IFACE_DIR)/bridge.c         \
            $(IFACE_DIR)/anim.c           \
            $(IFACE_DIR)/propwindow.c     \
            $(IFACE_DIR)/tradewindow.c    \
            $(IFACE_DIR)/pawn.c           \
            $(IFACE_DIR)/savegame.c       \
            $(IFACE_DIR)/pausemenu.c      \
            $(IFACE_DIR)/loadmenu.c       \
            $(IFACE_DIR)/rules.c          \
            $(UI_DIR)/button.c            \
            $(UI_DIR)/text.c              \
            $(UI_DIR)/textbox.c           \
            $(UI_DIR)/counter.c           \
            $(UI_DIR)/playercard.c        \
            $(UI_DIR)/checkbox.c

OBJ_MOD          = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRC_MOD))
OBJ_CTRL         = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRC_CTRL))
OBJ_CTRL_CONSOLE = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRC_CTRL_CONSOLE))
OBJ_IFACE        = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRC_IFACE))
OBJ_COMMON       = $(OBJ_MOD)

ALL_OBJ = $(OBJ_MOD) $(OBJ_CTRL) $(OBJ_IFACE)

TARGET      = $(BIN_DIR)/insapoly
TEST_EXEC   = $(BIN_DIR)/unit_tests
TEST_CARTES = $(BIN_DIR)/test_cartes
CONTROLLEUR = $(BIN_DIR)/controlleur

.DEFAULT_GOAL := all

all: $(TARGET)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(ALL_OBJ)
	$(CC) $^ -o $@ $(LIBS)
	@echo "✔  Compilation réussie → $(TARGET)"

test: $(BUILD_DIR)/model/test.o $(OBJ_COMMON)
	$(CC) $(CFLAGS) $^ -o $(TEST_EXEC) $(LDFLAGS) $(LIBS)

test_cartes: $(BUILD_DIR)/model/main_test_cartes.o $(OBJ_COMMON)
	$(CC) $(CFLAGS) $^ -o $(TEST_CARTES) $(LDFLAGS) $(LIBS)

controlleur: $(BUILD_DIR)/controller/main.o $(OBJ_CTRL_CONSOLE) $(OBJ_COMMON)
	$(CC) $(CFLAGS) $^ -o $(CONTROLLEUR) $(LDFLAGS) $(LIBS)

run: $(TARGET)
	@echo "Lancement depuis la racine…"
	./$(TARGET)

run_unit_tests: test
	./$(TEST_EXEC)

run_test: test_cartes
	./$(TEST_CARTES)

run_controlleur: controlleur
	./$(CONTROLLEUR)

doc:
	doxygen Doxyfile
	@echo "Documentation générée dans docs/html/"
	xdg-open docs/html/index.html

clean:
	rm -f $(ALL_OBJ)
	@echo "Fichiers objets supprimés"

.PHONY: all run run_unit_tests run_test run_controlleur doc clean mrproper