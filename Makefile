CC = gcc
CFLAGS = -Wall -Wextra -Werror 
LDFLAGS = -lcmocka
SRC = property.c player.c
OBJ = $(SRC:.c=.o)

EXEC = insapoly
TEST_EXEC = unit_tests

all: $(EXEC)

$(EXEC): main.o $(OBJ)
	$(CC) $(CFLAGS) main.o $(OBJ) -o $(EXEC)

test: test.o $(OBJ)
	$(CC) $(CFLAGS) test.o $(OBJ) -o $(TEST_EXEC) $(LDFLAGS)



%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o $(EXEC)