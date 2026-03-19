TARGET=insapoly

CC=gcc
CFLAGS=-Wall -Wextra $(shell sdl2-config --cflags)
LIBS=$(shell sdl2-config --libs) -lSDL2_image -lSDL2_ttf
SRC=main_view.c view.c

all:$(TARGET)

$(TARGET):$(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LIBS)

clean : 
	rm -f $(TARGET)

run: $(TARGET)
	./$(TARGET)