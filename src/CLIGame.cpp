/* Provide a basic "interface" for the game without graphics.
 *
 * Author: Joey Rees-Hill
 *
 * Date: April 2019
 */

#include <iostream>
#include <string>
#include "GameEngine.h"

using namespace std;

bool askKey(string keyName) {
    cout << "Is key " << keyName << " pressed? [y/n] ";
    string line;
    getline(cin, line);
    if (line.size())
        return 'y' == tolower(line[0]);
    return false;
}

int main() {
    GameEngine *game = new GameEngine();
    GameState gameState;
    InputState inp;
    do {
        inp.mainThruster = askKey("up");
        inp.rotLeftThruster = askKey("left");
        inp.rotRightThruster = askKey("right");
        gameState = game->step(inp);

        cout << "Fuel:      " << gameState.fuel << "\n";
        cout << "Ship Xpos: " << gameState.shipXPos << "\n";
        cout << "Ship Ypos: " << gameState.shipYPos << "\n";
        cout << "Ship Xvel: " << gameState.shipXVelocity << "\n";
        cout << "Ship Yvel: " << gameState.shipYVelocity << "\n";
        cout << "Ship rot:  " << gameState.shipRotation << "\n";
        cout << "Ship AngV: " << gameState.shipAngularVelocity << "\n";
    } while(!gameState.gameOver);

    delete game;
}
