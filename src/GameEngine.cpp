#include "GameEngine.h"
#include <iostream>

// Constructor
GameEngine::GameEngine() {
    this->fuel = 100;

    this->shipYPos = 800;
    this->shipXPos = -100;
    this->shipRotation = 0;

    this->shipYVel = 0;
    this->shipXVel = 0;
    this-> shipAngVel = 0;

    this->gameFinished = false;
}

// Step
struct GameState GameEngine::step(struct InputState input) {
    std::cout << "main: " << input.mainThruster << "\n";
    GameState gameState = {};
    return gameState;
}
