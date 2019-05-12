#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include "GUIGame.h"

using namespace std;

#define UNIT_SIZE 5

int main () {
  srand(time(0));
  
  InputState input[SCREEN_WIDTH/UNIT_SIZE][SCREEN_HEIGHT/UNIT_SIZE];

  for (int i=0 ; i < SCREEN_WIDTH/UNIT_SIZE; i++) {
    for (int k=0; k < SCREEN_HEIGHT/UNIT_SIZE; k++) {
      input[i][k] = InputState {rand() % 2, rand() % 2, rand() % 2};
    }
  }

  GameEngine g;
  
  while (!g.getFinished()) {
    int x = floor((g.getX() + SCREEN_WIDTH/2) / UNIT_SIZE);
    int y = floor((g.getY() + SCREEN_HEIGHT/2) / UNIT_SIZE);
    cout << x << " " << y << endl;
    InputState curr = input[x][y];
    
    g.step(curr);
  }
  cout << g.getScore() << endl;
  
  return 0;
}
