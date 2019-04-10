CPPC = g++
FLAGS = -g -pedantic -Wall -Wextra # -Werror
SDLFLAGS = `pkg-config --cflags sdl2` `pkg-config --libs sdl2`
SRC = src
BIN = bin
ALL = $(BIN)/hello $(BIN)/01_hello_SDL $(BIN)/02_image $(BIN)/03_events $(BIN)/04_keys

all: $(ALL)

$(BIN)/hello: $(SRC)/hello.cpp
	$(CPPC) $(FLAGS) $(SDLFLAGS) -o $(BIN)/hello $(SRC)/hello.cpp

$(BIN)/01_hello_SDL: $(SRC)/01_hello_SDL.cpp
	$(CPPC) $(FLAGS) $(SDLFLAGS) -o $(BIN)/01_hello_SDL -lSDL2 $(SRC)/01_hello_SDL.cpp

$(BIN)/02_image: $(SRC)/02_image.cpp
	$(CPPC) $(FLAGS) $(SDLFLAGS) -o $(BIN)/02_image -lSDL2 $(SRC)/02_image.cpp

$(BIN)/03_events: $(SRC)/03_events.cpp
	$(CPPC) $(FLAGS) $(SDLFLAGS) -o $(BIN)/03_events -lSDL2 $(SRC)/03_events.cpp

$(BIN)/04_keys: $(SRC)/04_keys.cpp
	$(CPPC) $(FLAGS) $(SDLFLAGS) -o $(BIN)/04_keys -lSDL2 $(SRC)/04_keys.cpp

clean:
	rm -f $(ALL)
	rm -rf $(BIN)/*.dSYM
