#include <iostream>
#include "GameEngine.h"

int main() {
    GameEngine *game = new GameEngine();
    struct GameState gameState;
    struct InputState inp = {true, false, false};
    gameState = game->step(inp);
    std::cout << gameState.shipYPos << "\n";

    inp.mainThruster = false;
    gameState = game->step(inp);
    std::cout << gameState.shipYPos << "\n";

    delete game;
}
