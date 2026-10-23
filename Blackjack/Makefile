CC = gcc
CFLAGS = -Wall -Wextra `sdl2-config --cflags`
LIBS = `sdl2-config --libs` -lSDL2_ttf

TARGET = blackjack
SRCS = main.c game.c render.c

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET) $(LIBS)

clean:
	rm -f $(TARGET)
