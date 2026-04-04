TARGET = insapoly

CC     = gcc
CFLAGS = -Wall -Wextra $(shell sdl2-config --cflags)
LIBS   = $(shell sdl2-config --libs) -lSDL2_image -lSDL2_ttf

SRC =   main_interface.c    \
        view.c              \
        events.c            \
        assets.c            \
        bridge.c            \
        adminWindow.c       \
        anim.c              \
        ui/button.c         \
        ui/text.c           \
        ui/textbox.c        \
        ui/counter.c        \
        ui/playercard.c     \
        ../Controlleur/game_engine.c \
        ../Controlleur/plateau.c     \
        ../Modele/player.c           \
        ../Modele/property.c

OBJ = $(SRC:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $@ $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run