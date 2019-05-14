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
#include "const.h"

using namespace std;

#define POPULATION_SIZE 100
#define UNIT_SIZE 200 // must divide SCREEN_WIDTH and HEIGHT
#define DEFAULT_FITNESS INT_MIN
#define TOURNAMENT_SIZE 3
#define ELITE_SIZE 2
#define GENERATIONS 20
#define MUTATION_PROBABILITY 20
#define CROSSOVER_PROBABILITY 80

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
        GP(void);
       ~GP(void);
        int popSize;
	int tournamentSize;
	int eliteSize;
	int generations;
	int mutationProbability;
	int crossoverProbability;
        
        vector<Individual *> pop;
        RandGen r;
        void initPop(void);
        void mutate(Individual *i);
        tuple<Individual *, Individual *> crossover(Individual *i1, Individual *i2);
	static void evaluate(Individual *);
	void sortPopulation(void);
	vector<Individual *> tournamentSelection(void);
	static void sortPopulation(vector<Individual *> p);
	void generationalReplacement(vector<Individual *> newPop, vector<Individual *> oldPop);
	static void evaluatePopulation(vector<Individual *> p);
	Individual *searchLoop(vector<Individual *> p);
};


#endif
