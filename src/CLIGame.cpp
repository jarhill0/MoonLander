#include <iostream>
#include <memory>
#include "GameEngine.h"

int main() {
    GameEngine *game = new GameEngine();
    std::unique_ptr<struct GameState> gameState;
    std::unique_ptr<struct InputState> inp = std::unique_ptr<struct InputState>
        (new struct InputState());
    inp->mainThruster = true;
    inp->rotLeftThruster = false;
    inp->rotRightThruster = false;
    gameState = game->step(inp);
    std::cout << gameState->shipYPos << "\n";

    inp->mainThruster = false;
    gameState = game->step(inp);
    std::cout << gameState->shipYPos << "\n";

    delete game;
}
