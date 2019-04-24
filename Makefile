CPPC = g++
FLAGS = -g -pedantic -Wall -Wextra -Werror -std=c++11
SDLFLAGS = `pkg-config --cflags --libs sdl2`
SDLIMGFLAGS = `pkg-config --cflags --libs sdl2_image`
SRC = src
BIN = bin
OBJ = obj
ALL = $(BIN)/GUIGame

ifneq ($(OS), Windows_NT)
	DIR = -p
endif

all: $(OBJ) $(BIN) $(ALL)

$(BIN):
	mkdir $(DIR) $(BIN)

$(OBJ):
	mkdir $(DIR) $(OBJ)

$(OBJ)/GameEngine.o: $(SRC)/GameEngine.cpp $(SRC)/GameEngine.h $(SRC)/const.h
	$(CPPC) $(FLAGS) -c -o $(OBJ)/GameEngine.o $(SRC)/GameEngine.cpp

$(OBJ)/GUIGame.o: $(SRC)/GUIGame.cpp $(SRC)/GameEngine.h
	$(CPPC) $(FLAGS) -c -o $(OBJ)/GUIGame.o $(SRC)/GUIGame.cpp

$(BIN)/GUIGame: $(OBJ)/GUIGame.o $(OBJ)/GameEngine.o
	$(CPPC) $(FLAGS) $(SDLFLAGS) $(SDLIMGFLAGS) -o $(BIN)/GUIGame $(OBJ)/GUIGame.o $(OBJ)/GameEngine.o

clean:
	rm -f $(ALL) $(OBJ)/*.o
	rm -rf $(BIN)/*.dSYM
