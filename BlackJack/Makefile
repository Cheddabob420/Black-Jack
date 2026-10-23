CC = gcc
CFLAGS = -Wall -Wextra `sdl2-config --cflags`
LIBS = `sdl2-config --libs` -lSDL2_ttf

TARGET = blackjack
SRC = main.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LIBS)

clean:
	rm -f $(TARGET)
