CPPC = g++
FLAGS = -g -pedantic -Wall -Wextra -Werror
SRC = src
BIN = bin
ALL = $(BIN)/hello

all: $(ALL)

$(BIN)/hello: $(SRC)/hello.cpp
	$(CPPC) $(FLAGS) -o $(BIN)/hello $(SRC)/hello.cpp

clean:
	rm -f $(ALL)
	rm -rf $(BIN)/*.dSYM
