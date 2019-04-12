#include "GameEngine.h"
#include <memory>
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
std::unique_ptr<struct GameState> GameEngine::step(
                std::unique_ptr<struct InputState> const &input) {
    std::cout << "main: " << input->mainThruster << "\n";
    return std::unique_ptr<struct GameState>(new struct GameState());
}
