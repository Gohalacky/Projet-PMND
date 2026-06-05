CC      = gcc
CFLAGS  = -Wall -Wextra -g -O0 --coverage -Isrc/Model -Isrc/Interface -Isrc/Controller $(shell sdl2-config --cflags)
LDFLAGS   = -lcmocka
LIBS      = $(shell sdl2-config --libs) -lSDL2_image -lSDL2_ttf -lm -lcjson
LIBS_BASE = -lm

SRC_DIR   = src
MOD_DIR   = src/Model
CTRL_DIR  = src/Controller
IFACE_DIR = src/Interface
UI_DIR    = src/Interface/ui
BUILD_DIR = build
BIN_DIR   = bin

SRC_MOD = $(MOD_DIR)/cartes.c       \
          $(MOD_DIR)/evenements.c   \
          $(MOD_DIR)/hypo.c         \
          $(MOD_DIR)/player.c       \
          $(MOD_DIR)/property.c     \
          $(MOD_DIR)/tool.c         \
          $(MOD_DIR)/trade.c        \
          $(MOD_DIR)/prison.c       \
          $(MOD_DIR)/voe.c

SRC_CTRL = $(CTRL_DIR)/game_engine.c \
           $(CTRL_DIR)/engine_logic.c \
           $(CTRL_DIR)/plateau.c

SRC_CTRL_CONSOLE = $(CTRL_DIR)/game_engine_console.c \
                   $(CTRL_DIR)/engine_logic.c        \
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
            $(IFACE_DIR)/prisonwindow.c   \
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
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/tests/%.o: tests/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(ALL_OBJ)
	mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)
	@echo "✔  Compilation réussie → $(TARGET)"

test: $(BUILD_DIR)/tests/test.o $(OBJ_COMMON)
	mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $(TEST_EXEC) $(LDFLAGS) $(LIBS_BASE)

test_cartes: $(BUILD_DIR)/tests/test_cartes_evenements.o $(OBJ_COMMON)
	mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $(TEST_CARTES) $(LDFLAGS) $(LIBS_BASE)

controlleur: $(BUILD_DIR)/Controller/main.o $(OBJ_CTRL_CONSOLE) $(OBJ_COMMON)
	mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $(CONTROLLEUR) $(LIBS_BASE)
	@echo "✔  Compilation réussie → $(CONTROLLEUR)"

run: $(TARGET)
	@echo "Lancement depuis la racine…"
	./$(TARGET)

run_unit_tests: test
	./$(TEST_EXEC)

run_test: test_cartes
	./$(TEST_CARTES)

run_controlleur: controlleur
	./$(CONTROLLEUR)

coverage: run_unit_tests run_test
	gcovr -r . --html-details -o rapport_global.html
	firefox rapport_global.html &

doc:
	doxygen Doxyfile
	@echo "Documentation générée dans docs/html/"
	xdg-open docs/html/index.html

valgrind_test: test
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(TEST_EXEC)

valgrind_cartes: test_cartes
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(TEST_CARTES)

valgrind_game: $(TARGET)
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --log-file="rapport_valgrind_test.txt" ./$(TARGET)
	@echo "Rapport généré dans rapport_valgrind_test.txt"
	xdg-open rapport_valgrind_test.txt

valgrind: valgrind_test valgrind_cartes

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)
	@echo "Fichiers objets, binaires et couverture supprimés"

mrproper: clean
	rm -rf docs/html
	rm -f rapport_global*.html *.css rapport_valgrind_test.txt *.xml

.PHONY: all run run_unit_tests run_test run_controlleur coverage doc clean mrproper