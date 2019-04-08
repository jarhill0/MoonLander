CPPC = g++
FLAGS = -g -pedantic -Wall -Wextra # -Werror
SRC = src
BIN = bin
ALL = $(BIN)/hello $(BIN)/01_hello_SDL $(BIN)/02_image

all: $(ALL)

$(BIN)/hello: $(SRC)/hello.cpp
	$(CPPC) $(FLAGS) -o $(BIN)/hello $(SRC)/hello.cpp

$(BIN)/01_hello_SDL: $(SRC)/01_hello_SDL.cpp
	$(CPPC) $(FLAGS) -o $(BIN)/01_hello_SDL -lSDL2 $(SRC)/01_hello_SDL.cpp

$(BIN)/02_image: $(SRC)/02_image.cpp
	$(CPPC) $(FLAGS) -o $(BIN)/02_image -lSDL2 $(SRC)/02_image.cpp

clean:
	rm -f $(ALL)
	rm -rf $(BIN)/*.dSYM
