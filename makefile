OBJS = src/tetris.cpp
CC = g++
LINKER_FLAGS = -lSDL2 -lSDL2_image
OBJ_NAME = tetris

all:
	$(CC) $(OBJS) $(CXXFLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME)
