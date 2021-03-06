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
#include <string.h>
#include "BitBuffer.h"
#include "GameEngine.h"
#include "const.h"

using namespace std;

#define MOON_TILE_HEIGHT 64

#define POPULATION_SIZE 250
#define UNIT_SIZE 40 // must divide SCREEN_WIDTH and HEIGHT
#define DEFAULT_FITNESS INT_MIN
#define TOURNAMENT_SIZE 50
#define ELITE_SIZE 20
#define GENERATIONS 1000
#define MUTATION_PROBABILITY 30
const int NUM_MUTATIONS = 150;
#define CROSSOVER_PROBABILITY 35
#define SURVIVE 70

const int VECT_W = SCREEN_WIDTH/UNIT_SIZE;
const int HALF_W = SCREEN_WIDTH/2;
const int VECT_H = SCREEN_HEIGHT/UNIT_SIZE;

class Individual {
    public:
        vector<vector<InputState>> inputs;
        double fitness;
	
        Individual(vector<vector<InputState>> inputs, double fitness);
	~Individual(void);
        void print(void);
};


class RandGen {
    mt19937 randGen;
    unsigned seed;

    public:
        RandGen(void);
        RandGen(unsigned seed);
        static unsigned genSeed(void);
        InputState randState(void);
        int randInt(int min, int max);
        bool randBool(void);
	tuple<int,int,int,int> randIndices(Individual *i);

	bool randBool75(void);
};



class GP {
    public:
        GP(void);
	GP(FILE *o, FILE *g);
       ~GP(void);
       
        int popSize;
	int tournamentSize;
	int eliteSize;
	int generations;
	int mutationProbability;
	int crossoverProbability;
	int survive; 
        vector<Individual *> pop;
        RandGen r;
	FILE *output, *grid;

	void defaultInit();
        void initPop(void);
        void mutate(Individual *i);
        tuple<Individual *, Individual *> crossover(Individual *i1, Individual *i2);
	void evaluate(Individual *i, bool print=false);
    void dumpGrid(Individual *i);
	vector<Individual *> tournamentSelection(vector<Individual *> p);
	static void sortPopulation(vector<Individual *> &p);
	void generationalReplacement(vector<Individual *> newPop, vector<Individual *> oldPop);
        void evaluatePopulation(vector<Individual *> &p);
	Individual *searchLoop(vector<Individual *> p);
};


#endif
