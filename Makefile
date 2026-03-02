CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c11 -g -I. 
LDFLAGS = -lcmocka
SRC = property.c player.c
OBJ = $(SRC:.c=.o)

EXEC = insapoly
TEST_EXEC = unit_tests

all: $(EXEC)

$(EXEC): main.o $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@

test: test.o $(OBJ)
	$(CC) $(CFLAGS) $^ -o $(TEST_EXEC) $(LDFLAGS)


%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o $(EXEC) $(TEST_EXEC)