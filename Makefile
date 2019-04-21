CPPC = g++
FLAGS = -g -pedantic -Wall -Wextra -Werror -std=c++11
SDLFLAGS = `pkg-config --cflags --libs sdl2`
SRC = src
BIN = bin
OBJ = obj
ALL = $(BIN)/CLIGame

all: $(OBJ) $(BIN) $(ALL)

$(BIN):
	mkdir -p $(BIN)

$(OBJ):
	mkdir -p $(OBJ)

$(OBJ)/GameEngine.o: $(SRC)/GameEngine.cpp $(SRC)/GameEngine.h
	$(CPPC) $(FLAGS) -c -o $(OBJ)/GameEngine.o $(SRC)/GameEngine.cpp

$(OBJ)/CLIGame.o: $(SRC)/CLIGame.cpp $(SRC)/GameEngine.h
	$(CPPC) $(FLAGS) -c -o $(OBJ)/CLIGame.o $(SRC)/CLIGame.cpp

$(BIN)/CLIGame: $(OBJ)/GameEngine.o $(OBJ)/CLIGame.o
	$(CPPC) $(FLAGS) -o $(BIN)/CLIGame $(OBJ)/GameEngine.o $(OBJ)/CLIGame.o

clean:
	rm -f $(ALL) $(OBJ)/*.o
	rm -rf $(BIN)/*.dSYM
