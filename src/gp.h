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


using namespace std;

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define MOON_TILE_HEIGHT 64

#define POPULATION_SIZE 5
#define UNIT_SIZE 40 // must divide SCREEN_WIDTH and HEIGHT
#define DEFAULT_FITNESS INT_MIN
#define TOURNAMENT_SIZE 3
#define ELITE_SIZE 2
#define GENERATIONS 5
#define MUTATION_PROBABILITY 5
#define CROSSOVER_PROBABILITY 80
#define SURVIVE 70

const int VECT_W = SCREEN_WIDTH/UNIT_SIZE;
const int HALF_W = SCREEN_WIDTH/2;
const int VECT_H = SCREEN_HEIGHT/UNIT_SIZE;

class Individual {
    public:
        vector<vector<InputState *>> inputs;
        double fitness;
	
        Individual(vector<vector<InputState *>> inputs, double fitness);
	~Individual(void);
	Individual(const Individual &i);
	Individual &operator=(Individual i);
	bool operator<(const Individual &i);
	static bool compare(const Individual& i1, const Individual& i2);
        void print(void);
};


class RandGen {
    mt19937 randGen;
    unsigned seed;

    public:
        RandGen(void);
        RandGen(unsigned seed);
        static unsigned genSeed(void);
        InputState *randState(void);
        int randInt(int min, int max);
        bool randBool(void);
	tuple<int,int,int,int> randIndices(Individual *i);

	bool randBool75(void);
};



class GP {
    public:
        GP(void);
	GP(FILE *o);
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
	FILE *output;
	BitBuffer *buffer;

	void defaultInit();
        void initPop(void);
        void mutate(Individual *i);
        tuple<Individual *, Individual *> crossover(Individual *i1, Individual *i2);
	void evaluate(Individual *i, bool print=false);
	void sortPopulation(void);
	vector<Individual *> tournamentSelection(vector<Individual *> p);
	static void sortPopulation(vector<Individual *> p);
	void generationalReplacement(vector<Individual *> newPop, vector<Individual *> oldPop);
        void evaluatePopulation(vector<Individual *> p);
	Individual *searchLoop(vector<Individual *> p);
};


#endif
