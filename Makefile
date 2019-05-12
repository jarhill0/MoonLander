CPPC = g++
FLAGS = -g -pedantic -Wall -Wextra -std=c++11
SDLFLAGS = `pkg-config --cflags --libs sdl2`
SDLIMGFLAGS = `pkg-config --cflags --libs sdl2_image`
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

$(OBJ)/GUIGame.o: $(SRC)/GUIGame.cpp $(SRC)/GameEngine.h $(SRC)/BitBuffer.h $(SRC)/GUIGame.h
	$(CPPC) $(FLAGS) -c -o $(OBJ)/GUIGame.o $(SRC)/GUIGame.cpp

$(BIN)/GUIGame: $(OBJ)/GUIGame.o $(OBJ)/GameEngine.o $(OBJ)/BitBuffer.o
	$(CPPC) -o $(BIN)/GUIGame $(OBJ)/GUIGame.o $(OBJ)/GameEngine.o $(OBJ)/BitBuffer.o $(FLAGS) $(SDLIMGFLAGS) $(SDLFLAGS)

$(OBJ)/GP.o: $(SRC)/gp.cpp $(SRC)/GameEngine.h $(SRC)/BitBuffer.h
	$(CPPC) $(FLAGS) -c -o $(OBJ)/GP.o $(SRC)/gp.cpp

$(BIN)/GP: $(OBJ)/GUIGame.o $(OBJ)/GameEngine.o $(OBJ)/BitBuffer.o $(OBJ)/GP.o
	$(CPPC) -o $(BIN)/GP $(OBJ)/GP.o $(OBJ)/GameEngine.o $(OBJ)/BitBuffer.o $(FLAGS) $(SDLIMGFLAGS) $(SDLFLAGS)


$(OBJ)/BitBuffer.o: $(SRC)/BitBuffer.cpp $(SRC)/BitBuffer.h
	$(CPPC) $(FLAGS) -c -o $(OBJ)/BitBuffer.o $(SRC)/BitBuffer.cpp

clean:
	rm -f $(ALL) $(OBJ)/*.o
	rm -rf $(BIN)/*.dSYM
