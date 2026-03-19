CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c11 -g -I. 
LDFLAGS = -lcmocka
SRC = property.c player.c tool.c eco.c cartes.c alea.c evenements.c
OBJ = $(SRC:.c=.o)

EXEC = insapoly
TEST_EXEC = unit_tests
TEST_CARTES = test_cartes

all: $(EXEC)

$(EXEC): main.o $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@

test: test.o $(OBJ)
	$(CC) $(CFLAGS) $^ -o $(TEST_EXEC) $(LDFLAGS)

test_cartes : main_test_cartes.o $(OBJ)
	$(CC) $(CFLAGS) $^ -o $(TEST_CARTES) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o $(EXEC) $(TEST_EXEC) $(TEST_CARTES)