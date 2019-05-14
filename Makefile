CPPC = g++
FLAGS = -g -pedantic -Wall -Wextra -Werror -std=c++11
SDLFLAGS = `pkg-config --cflags --libs sdl2 sdl2_image sdl2_ttf`

SRC = src
BIN = bin
OBJ = obj
ALL = $(BIN)/GUIGame $(BIN)/GP

all: $(OBJ) $(BIN) $(ALL)

$(BIN):
	mkdir -p $(BIN)

$(OBJ):
	mkdir -p $(OBJ)

$(OBJ)/GameEngine.o: $(SRC)/GameEngine.cpp $(SRC)/GameEngine.h $(SRC)/const.h
	$(CPPC) $(FLAGS) -c -o $(OBJ)/GameEngine.o $(SRC)/GameEngine.cpp

$(OBJ)/GUIGame.o: $(SRC)/GUIGame.cpp $(SRC)/GameEngine.h $(SRC)/BitBuffer.h $(SRC)/const.h
	$(CPPC) $(FLAGS) -c -o $(OBJ)/GUIGame.o $(SRC)/GUIGame.cpp

$(BIN)/GUIGame: $(OBJ)/GUIGame.o $(OBJ)/GameEngine.o $(OBJ)/BitBuffer.o
	$(CPPC) $(FLAGS) -o $(BIN)/GUIGame $(OBJ)/GUIGame.o $(OBJ)/GameEngine.o $(OBJ)/BitBuffer.o $(SDLFLAGS)

$(OBJ)/GP.o: $(SRC)/gp.cpp $(SRC)/GameEngine.h $(SRC)/BitBuffer.h $(SRC)/gp.h
	$(CPPC) $(FLAGS) -c -o $(OBJ)/GP.o $(SRC)/gp.cpp

$(BIN)/GP: $(OBJ)/GameEngine.o $(OBJ)/BitBuffer.o $(OBJ)/GP.o
	$(CPPC) -o $(BIN)/GP $(OBJ)/GP.o $(OBJ)/GameEngine.o $(OBJ)/BitBuffer.o $(FLAGS)

$(OBJ)/BitBuffer.o: $(SRC)/BitBuffer.cpp $(SRC)/BitBuffer.h
	$(CPPC) $(FLAGS) -c -o $(OBJ)/BitBuffer.o $(SRC)/BitBuffer.cpp

clean:
	rm -f $(ALL) $(OBJ)/*.o
	rm -rf $(BIN)/*.dSYM
