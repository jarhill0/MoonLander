#ifndef GP_H
#define GP_H

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <vector>
#include <tuple>
#include <random>
#include <climits>
#include <assert.h>
#include <memory>
#include <algorithm>
#include "GameEngine.h"


using namespace std;

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

#define POP_SIZE 5
#define UNIT_SIZE 200 // must divide SCREEN_WIDTH and HEIGHT
#define DEFAULT_FITNESS INT_MIN

const int VECT_W = SCREEN_WIDTH/UNIT_SIZE;
const int HALF_W = SCREEN_WIDTH/2;
const int VECT_H = SCREEN_HEIGHT/UNIT_SIZE;

class Individual {
    public:
        vector<vector<InputState *>> inputs;
        double fitness;
	
        Individual(vector<vector<InputState *>> inputs, double fitness);
	~Individual(void);
	Individual(Individual &i);
        void print(void);
};


class RandGen {
    mt19937 rand_gen;
    unsigned seed;

    public:
        RandGen(void);
        RandGen(unsigned seed);
        static unsigned genSeed(void);
        InputState *randState(void);
        int randInt(int min, int max);
        bool randBool(void);
	tuple<int,int,int,int> randIndices(Individual *i);
};



class GP {
    public:
        GP(int size);
       ~GP(void);
        int pop_size;
        
        vector<Individual *> pop;
        RandGen r;
        void initPop(void);
        void mutate(Individual *i);
        tuple<Individual *, Individual *> crossover(Individual *i1, Individual *i2);
	void evaluate(Individual *);
	
	void sortPopulation(void);
};


#endif
