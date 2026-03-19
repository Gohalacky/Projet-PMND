CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c11 -g -I. -IInterface -IModele
LDFLAGS = -lcmocka
LIBS = $(shell sdl2-config --libs) -lSDL2_image -lSDL2_ttf

INTERFACE_DIR = Interface
MODELE_DIR    = Modele

SRC_ROOT      = property.c player.c tool.c eco.c cartes.c alea.c evenements.c
SRC_INTERFACE = $(INTERFACE_DIR)/main_view.c $(INTERFACE_DIR)/view.c
SRC_MODELE    = $(MODELE_DIR)/main.c $(MODELE_DIR)/player.c $(MODELE_DIR)/property.c

SRC = $(SRC_ROOT) $(SRC_INTERFACE) $(SRC_MODELE)
OBJ = $(SRC:.c=.o)

EXEC         = insapoly
TEST_EXEC    = unit_tests
TEST_CARTES  = test_cartes
VIEW         = interface

all: $(EXEC)

$(EXEC): main.o $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

test: test.o $(OBJ)
	$(CC) $(CFLAGS) $^ -o $(TEST_EXEC) $(LDFLAGS)

test_cartes: main_test_cartes.o $(OBJ)
	$(CC) $(CFLAGS) $^ -o $(TEST_CARTES) $(LDFLAGS)

interface: $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(VIEW) $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o $(INTERFACE_DIR)/*.o $(MODELE_DIR)/*.o \
	      $(EXEC) $(TEST_EXEC) $(TEST_CARTES)