/* Provide the game engine.
 *
 * Author: Joey Rees-Hill
 *
 * Date: April 2019
 */

#include "GameEngine.h"
#include "const.h" // also includes math.h

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
            calculateScore();
        }
    }

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

double GameEngine::calculateScore() {
    gameFinished = true;
    score = 0.0;
    score += IMPACT_FRAC/(1+pow(shipYVel/6,2));
    score += UPRIGHT_FRAC/(1+pow((shipRotation - M_PI / 2),2));

    return score;
}
