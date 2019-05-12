#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include "GUIGame.h"
#include <vector>

#define UNIT_SIZE 5

using namespace std;

void init_pop(void);


struct Individual {
  vector<vector<InputState *>> inputs;
  
  double fitness;
};

const int pop_size = 100;

const int vect_w = SCREEN_WIDTH/UNIT_SIZE;
const int vect_h = SCREEN_HEIGHT/UNIT_SIZE;
const int half_w = SCREEN_WIDTH/2;

vector<Individual *> pop(pop_size);

int main () {
  srand(time(0));
  
  init_pop();
  
  for (auto i : pop) {
    
    GameEngine g;

    // NEED TO KNOW MOONTILE HEIGHT FOR THIS!!!
    g.setBounds(-(SCREEN_WIDTH / 2), SCREEN_WIDTH / 2,
		   SCREEN_HEIGHT);

    while (!g.getFinished()) {
      int x = floor((g.getX() + half_w) / UNIT_SIZE);
      int y = floor((SCREEN_HEIGHT - g.getY()) / UNIT_SIZE);

      g.step(*(i->inputs[x][y]));
    }

    i -> fitness = g.getScore();
    cout << i -> fitness << endl;
  }
  
  return 0;
}

void init_pop() {
  for (int i=0; i < (int)pop.size(); i++) {
    Individual *new_ind = new Individual;

    vector<vector<InputState *>> inps(vect_w);

    for (int r = 0; r < vect_w; r++) {
      inps[r] = vector<InputState *>(vect_h);

      for (int c = 0; c < vect_h; c++) {
	inps[r][c] = new InputState { rand() % 2, rand() % 2, rand() % 2};
      }
    }

    new_ind->fitness = 0;
    new_ind->inputs = inps;

    pop[i] = new_ind;
  }
}
