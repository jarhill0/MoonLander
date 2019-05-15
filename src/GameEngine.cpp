/* Provide the game engine.
 *
 * Author: Joey Rees-Hill, William Knowles-Kellett
 *
 * Date: April 2019
 */

#include "GameEngine.h"
#include "const.h" // also includes math.h
#include <tuple>
#include <iostream>

InputState::InputState(bool m, bool l, bool r) {
    mainThruster = m;
    rotLeftThruster = l;
    rotRightThruster = r;
}

InputState::InputState(){}

void InputState::print() {
    std::cout << mainThruster << rotLeftThruster << rotRightThruster;
}

// Constructor
GameEngine::GameEngine() {
    fuel = START_FUEL;

    shipYPos = START_Y_POS;
    shipXPos = START_X_POS;
    shipRotation = START_ROT;

    shipYVel = START_Y_VEL;
    shipXVel = START_X_VEL;
    shipAngVel = START_ANG_VEL;

    gameFinished = false;
    score = 0;

    boundsSet = false;
    leftBound = 0;
    rightBound = 0;
    topBound = 0;
}

// Step
GameState GameEngine::step(InputState input) {
    if (!gameFinished) {
        double yAcc = -GRAV_ACC;
        double xAcc = 0;
        double angAcc = 0;

        if (fuel > 0) {
            if (input.mainThruster) {
                fuel -= MAIN_THRUST_FUEL_USE;
                xAcc += MAIN_THRUST_ACC * cos(shipRotation);
                yAcc += MAIN_THRUST_ACC * sin(shipRotation);
            }

            if (input.rotLeftThruster) {
                fuel -= SIDE_THRUST_FUEL_USE;
                angAcc += SIDE_THRUST_ACC;
            }
            if (input.rotRightThruster) {
                fuel -= SIDE_THRUST_FUEL_USE;
                angAcc -= SIDE_THRUST_ACC;
            }
        }

        shipAngVel += angAcc;
        shipYVel += yAcc;
        shipXVel += xAcc;

        applyVelocity();

        if (shipYPos <= 0) {
            shipYPos = 0;
            gameFinished = true;
            calculateScore();
        } else if (boundsSet && !checkBounds()) {
            fuel = 0;
            gameFinished = true;
            calculateScore();
        }
    }

    return makeState();
}

GameState GameEngine::getState() {
    return makeState();
}

GameState GameEngine::makeState() {
    return {
        fuel,  // fuel
        shipYPos,  // shipYPos
        shipXPos,  // shipXPos
        shipYVel,  // shipYVelocity
        shipXVel,  // shipXVelocity
        shipRotation,  // shipRotation
        shipAngVel,  // shipAngularVelocity
        gameFinished,  // gameOver
        score,  // score
    };
}

void GameEngine::applyVelocity() {
    shipYPos += shipYVel;
    shipXPos += shipXVel;
    shipRotation = fmod(shipRotation +  shipAngVel, 2 * M_PI);
}

void GameEngine::calculateScore() {
    // all component values should be positive
    double xDiff = shipXPos;
    xDiff /= 20;
    xDiff *= xDiff;
    double shipVel = sqrt(shipXVel * shipXVel + shipYVel * shipYVel);
    shipVel *= 30;
    double upright = shipRotation - M_PI / 2;
    upright = upright > 0 ? upright : -upright;
    upright *= 20;
    double shipRotVel = shipAngVel;
    shipRotVel *= 10;
    double heightDifference = 6 * shipYPos;  // needed because bounds check

    double distFromPerfect = xDiff + shipVel + upright + shipRotVel
        + heightDifference;
    score = fuel - (distFromPerfect * distFromPerfect) / 50;
}

// Set a boundary, outside of which the game will end.
void GameEngine::setBounds(int left, int right, int top) {
    leftBound = left;
    rightBound = right;
    topBound = top;
    boundsSet = true;
}

bool GameEngine::checkBounds() {
    return shipXPos >= leftBound &&
        shipXPos <= rightBound &&
        shipYPos <= topBound;
}

