CC = g++
CFLAGS = -std=c++17 -Wall
LIBS = -L"lib" -lSDL2main -lSDL2 -lSDL2_ttf -lSDL2_image -lSDL2_mixer
INCLUDES = -I"include"
EXEC = chasse

.PHONY: all force

all: force $(EXEC)

force:
	@if [ -f $(EXEC) ]; then rm -f $(EXEC); fi
	@true

$(EXEC): main.cpp
	$(CC) main.cpp $(CFLAGS) $(LIBS) $(INCLUDES) -o $(EXEC)

clean:
	rm -f $(EXEC)
