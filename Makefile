CC = gcc
CFLAGS = -Wall -Wextra -Werror 
OBJ = main.o property.o
EXEC = insapoly

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(OBJ) -o $(EXEC)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o $(EXEC)