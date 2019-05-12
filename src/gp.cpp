#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <vector>
#include <tuple>
#include "GUIGame.h"

#define UNIT_SIZE 40

using namespace std;

struct Individual {
  vector<vector<InputState *>> inputs;
  
  double fitness;
};


void init_pop(void);
InputState *rand_state(void);
void mutate(Individual *i);
tuple<Individual *, Individual *> crossover(Individual *i1, Individual *i2);

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
	inps[r][c] = rand_state();
      }
    }

    new_ind->fitness = 0;
    new_ind->inputs = inps;

    pop[i] = new_ind;
  }
}

void mutate(Individual *ind) {
  int r1 = rand() % (vect_w + 1);
  int c1 = rand() % (vect_h + 1);

  int r2 = rand() % (vect_w + 1);
  int c2 = rand() % (vect_h + 1);
  if (r1 > r2) {
    int tmp = r1;
    r1 = r2;
    r2 = tmp;
  }

  if (c1 > c2) {
    int tmp = c1;
    c1 = c2;
    c2 = tmp;
  }

  for (int i = r1; i < r2; i++) {
    for (int k = c1; k < c2; k++) {
           
      ind->inputs[i][k]->mainThruster = rand() % 2;
      ind->inputs[i][k]->rotLeftThruster = rand() % 2;
      ind->inputs[i][k]->rotRightThruster = rand() % 2;
    }
  }
}

tuple<Individual *, Individual *> crossover(Individual *i1, Individual *i2) {
  tuple <Individual *, Individual *> results;

  Individual *new_i1 = new Individual {i1->inputs, i1->fitness};
  Individual *new_i2 = new Individual {i2->inputs, i2->fitness};
  
  int r1 = rand() % (vect_w + 1);
  int c1 = rand() % (vect_h + 1);

  int r2 = rand() % (vect_w + 1);
  int c2 = rand() % (vect_h + 1);

  if (r1 > r2) {
    int tmp = r1;
    r1 = r2;
    r2 = tmp;
  }
  
  if (c1 > c2) {
    int tmp = c1;
    c1 = c2;
    c2 = tmp;
  }

  for (int i = r1; i < r2; i++) {
    for (int k = c1; k < c2; k++) {
      InputState *tmp = new_i1->inputs[i][k];

      *(new_i1->inputs[i][k]) = *(new_i2->inputs[i][k]);
      *(new_i2->inputs[i][k]) = *tmp;
    }
  }

  results = make_tuple(new_i1, new_i2);

  return results;
}

InputState *rand_state() {
  return new InputState {rand() % 2, rand() % 2, rand() % 2};
}

