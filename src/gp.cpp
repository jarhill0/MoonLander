#include "gp.h"

using namespace std;

FILE *getOutputFile(int argc, char *argv[]);


int main (int argc, char *argv[]) {
    assert(SCREEN_WIDTH % UNIT_SIZE == 0);
    assert(SCREEN_HEIGHT % UNIT_SIZE == 0);

    GP *gp = new GP(getOutputFile(argc, argv));
    
    Individual *bestEver = gp->searchLoop(gp->pop);

    if (gp->output) gp->evaluate(bestEver, true);

    cout << bestEver -> fitness << endl;
    
    for (Individual *i : gp->pop) {
 	delete i;
    }

    gp->pop.clear();

    if (gp->output) {
	gp->buffer->flush();
	fclose(gp->output);
	
	delete gp->buffer;
    }

    delete gp;

    exit(EXIT_SUCCESS);
}

FILE *getOutputFile(int argc, char *argv[]) {
    for (int i=1; i < argc; i++) {
	if (!strcmp(argv[i], "-o")) {
	    FILE *output = fopen(argv[i+1], "w");

	    if (!output) {
		fprintf(stderr, "Error opening output file.\n");

		exit(EXIT_FAILURE);
	    }

	    return output;
	}
    }

    return nullptr;
}

Individual::Individual(vector<vector<InputState *>> inps, double fit) {
    inputs = inps;
    fitness = fit;
}

Individual::Individual(const Individual &i) {
   vector<vector<InputState *>> new_vec(i.inputs.size());
    
    for (vector<InputState *> j: i.inputs) {
	vector<InputState *> n(j.size());
	
	for (InputState *k : j) {
	    InputState *new_inp = new InputState
		{k->mainThruster, k->rotLeftThruster, k->rotRightThruster};
	    n.push_back(new_inp);
	}

	new_vec.push_back(n);
    }

    inputs = new_vec;
    fitness = i.fitness;
}

Individual& Individual::operator=(Individual i) {
    swap(fitness, i.fitness);

    vector<vector<InputState *>> larger = (inputs.size() > i.inputs.size() ? inputs : i.inputs);
    vector<vector<InputState *>> smaller = (larger == inputs ? i.inputs : inputs);

    vector<vector<InputState *>> tmp(smaller.size());
    
    for (int k = 0; k < (int)smaller.size(); k++) {
	tmp[k] = smaller[k];
    }

    smaller.resize(larger.size());

    for (int k = 0; k < (int)larger.size(); k++) {
	swap(smaller[k], larger[k]);
    }

    larger.resize(tmp.size());

    for (int k = 0; k < (int)tmp.size(); k++) {
	swap(tmp[k], larger[k]);
    }

    return *this;
}

void Individual::print() {
    cout << "Fitness: " << fitness << endl;
    for (vector<InputState *> j : inputs) {
	for (InputState *k : j) {
	    k->print();
	    cout << " ";
	}
	
	cout << endl;
    }
}

Individual::~Individual() {
    for (vector<InputState *> j : inputs) {
    	for (InputState *k : j) {
    	    delete k;
    	}
    }
    
    inputs.clear();
}

RandGen::RandGen() {
    seed = genSeed();
    randGen.seed(seed);
}

RandGen::RandGen(unsigned s) {
    seed = s;
    randGen.seed(seed);
}

InputState *RandGen::randState() {
    return new InputState {randBool75(), !randBool75(), !randBool75()};
}

int RandGen::randInt(int min, int max) {
    uniform_int_distribution<std::mt19937::result_type> n(min, max);
    return n(randGen);
}

bool RandGen::randBool() {
    return (bool) randInt(0,1);
}

bool RandGen::randBool75() {
    return !(randBool() | randBool());
}

unsigned RandGen::genSeed() {
    return random_device()();
}

tuple<int, int, int, int> RandGen::randIndices(Individual *i) {
    int max_r = i->inputs.size();
    int max_c = i->inputs[0].size();
    
    int r1 = randInt(0, max_r-1);
    int c1 = randInt(0, max_c-1);
    int r2, c2;
    
    if (r1 == max_r) r2 = max_r+1;
    else r2 = randInt(r1+1, max_r);
    
    if (c1 == max_c) c2 = max_c+1;
    else c2 = randInt(c1+1, max_c);

    return make_tuple(r1, c1, r2, c2);
}

GP::GP() {
    output = nullptr;
    buffer = nullptr;
    defaultInit();
}

GP::GP(FILE *o) {
    output = o;
    buffer = new BitBuffer(output);
    defaultInit();
}

void GP::defaultInit() {
    popSize = POPULATION_SIZE;
    tournamentSize = TOURNAMENT_SIZE;
    eliteSize = ELITE_SIZE;
    generations = GENERATIONS;
    mutationProbability = MUTATION_PROBABILITY;
    crossoverProbability = CROSSOVER_PROBABILITY;
    survive = SURVIVE;
    
    pop.reserve(popSize);
    initPop();

}

GP::~GP() {
    for (Individual *i : pop) {
 	delete i;
    }

    pop.clear();
}

void GP::initPop() {
    for (int i = 0; i < popSize; i++) {
	vector<vector<InputState *>> inps(VECT_W);

	for (int k = 0; k < VECT_W; k++) {
	    inps[k] = vector<InputState *>(VECT_H);

	    for (int c = 0; c < VECT_H; c++) {
		inps[k][c] = r.randState();
	    }
	}
	
	pop.push_back(new Individual(inps, DEFAULT_FITNESS));
    }
}

void GP::mutate(Individual *ind) {
    if (r.randInt(0, 100) >= mutationProbability) return;

    tuple<int,int,int,int> indices = r.randIndices(ind);

    int r1 = get<0>(indices);
    int c1 = get<1>(indices);
    int r2 = get<2>(indices);
    int c2 = get<3>(indices);
    
    for (int i = r1; i < r2; i++) {
	for (int k = c1; k < c2; k++) {
	    ind->inputs[i][k]->mainThruster = r.randBool75();
	    ind->inputs[i][k]->rotLeftThruster = !r.randBool75();
	    ind->inputs[i][k]->rotRightThruster = !r.randBool75();
	}
    }
}

tuple<Individual *, Individual *> GP::crossover(Individual *i1, Individual *i2) {
    assert(i1->inputs.size() == i2->inputs.size());
    assert(i2->inputs[0].size() == i2->inputs[0].size());

    tuple<Individual *, Individual *> results;
    
    Individual *new_i1 = i1;
    Individual *new_i2 = i2;

    if (r.randInt(0, 100) < crossoverProbability) {
	tuple<int,int,int,int> indices = r.randIndices(i1);
	
	int r1 = get<0>(indices);
	int c1 = get<1>(indices);
	int r2 = get<2>(indices);
	int c2 = get<3>(indices);
	
	for (int i = r1; i < r2; i++) {
	    for (int k = c1; k < c2; k++) {
		
		InputState tmp = *(new_i1->inputs[i][k]);
		
		*(new_i1->inputs[i][k]) = *(new_i2->inputs[i][k]);
		*(new_i2->inputs[i][k]) = tmp;
	    }
	}
    }
	
    results = make_tuple(new_i1, new_i2);
    
    return results;
}

void GP::evaluate(Individual *i, bool print) {
    GameEngine g;

    g.setDefaultFields();
    g.setBounds(-(HALF_W), HALF_W, SCREEN_HEIGHT - MOON_TILE_HEIGHT);
    
    GameState gs = g.getState();
    
    while (!gs.gameOver) {
	
	int x = gs.shipXPos + HALF_W;
	int y = gs.shipYPos;
	
	x = floor(x / UNIT_SIZE);
	y = floor(y / UNIT_SIZE);

	InputState *is = i->inputs[x][y];
	
	if (print) {
	    if (!output || !buffer) {
		fprintf(stderr, "Output file undefined or buffer undefined");
	    } else {
		if (is->mainThruster == 0x0) cout << "0";
		else cout << "1";
		if (is->rotLeftThruster == 0x0) cout << "0";
		else cout << "1";
		if (is->rotRightThruster == 0x0) cout << "0";
		else cout << "1";


		buffer->putBit(is->mainThruster);
		buffer->putBit(is->rotLeftThruster);
		buffer->putBit(is->rotRightThruster);
	    }
	}
	
	gs = g.step(*is);
    }
    
    i -> fitness = gs.score;   
}

bool compareIndividuals(Individual *i1, Individual *i2) {
    return (i1->fitness < i2->fitness);
}


void GP::sortPopulation(vector<Individual *> p) {
    std::sort(p.begin(), p.end(), compareIndividuals);
}

vector<Individual *> GP::tournamentSelection(vector<Individual *> p) {
    vector<Individual *> winners(popSize);

    vector<Individual *> competitors(tournamentSize);

    int win_i = 0;

    while (win_i < popSize) {
	for (int i = 0; i < tournamentSize; i++) {
	    int idx = r.randInt(0, popSize - i - 1);
	    competitors[i] = p[idx]; 
	    
	    Individual *tmp = p[popSize - i - 1];
	    p[popSize - i - 1] = p[idx];
	    p[idx] = tmp;
	}

	sortPopulation(competitors);
	winners[win_i++] = competitors[0];
    }

    return winners;
}

void GP::generationalReplacement(vector<Individual *> newPop, vector<Individual *> oldPop) {
    sortPopulation(newPop);
    sortPopulation(oldPop);

    for (int i=0; i < eliteSize; i++) {
	// Elite individuals are always propogated
	*(newPop[popSize - i - 1]) = *(oldPop[i]);
	oldPop[i] = nullptr;
    }
}

Individual *GP::searchLoop(vector<Individual *> p) {
    evaluatePopulation(p);

    sortPopulation(p);

    Individual *bestEver = p[0];

    int gen = 1;

    while (gen < generations) {
	vector<Individual *> parents;
	vector<Individual *> newPop(popSize);

	int newPopIndex = 0;

	parents = tournamentSelection(p);

	// Crossover
	while (newPopIndex < popSize) {
	    int idx = r.randInt(0, popSize - 1);
	    
	    Individual *p1;
	    Individual *p2;

	    p1 = parents[idx];

	    p2 = parents[r.randInt(0, popSize - 2)];

	    assert(p1 != nullptr && p2 != nullptr);
	    
	    tuple<Individual *,  Individual *> children = crossover(p1, p2);
	    evaluate(get<0>(children));

	    // Append first child to population.

	    newPop[newPopIndex++] = get<0>(children);

	    // Wait to add the second child to ensure that you are not adding one too many.
	    // This is specifically for odd number population sizes.
	    if (newPopIndex < popSize) {
		evaluate(get<1>(children));
		newPop[newPopIndex++] = get<1>(children);
	    }
	}

	for (Individual *i : newPop) {
	    mutate(i);
	}

	evaluatePopulation(newPop);

	generationalReplacement(newPop, p);

	p.swap(newPop);

	sortPopulation(p);

	*bestEver = *p[0];
	
	cout << bestEver -> fitness << endl;

	gen++;
    }

    return bestEver;
}

void GP::evaluatePopulation(vector<Individual *> p) {
    for (Individual *ind : p) {
	evaluate(ind);
    }
}



