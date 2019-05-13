#include "gp.h"

using namespace std;


int main () {
    assert(SCREEN_WIDTH % UNIT_SIZE == 0);
    assert(SCREEN_HEIGHT % UNIT_SIZE == 0);    
	
    GP gp(POP_SIZE);

    for (Individual *i : gp.pop) {
	gp.evaluate(i);
	i -> print();
    }

    cout << "####### Crossover Example #######" << endl;
    gp.pop[0] -> print();
    gp.pop[1] -> print();

    tuple<Individual *, Individual *> tmp = gp.crossover(gp.pop[0], gp.pop[1]);
    gp.evaluate(get<0>(tmp));
    gp.evaluate(get<1>(tmp));
    
    get<0>(tmp) -> print();
    get<1>(tmp) -> print();

    gp.sortPopulation();

    for (Individual *i : gp.pop) {
    	i -> print();
    }

    
    cout << "####### Mutation Example #######" << endl;

    gp.pop[2] -> print();

    gp.mutate(gp.pop[2]);

    gp.evaluate(gp.pop[2]);

    gp.pop[2] -> print();

    for (Individual *i : gp.pop) {
 	delete i;
    }

    gp.pop.clear();

    return 0;
}


Individual::Individual(vector<vector<InputState *>> inps, double fit) {
    inputs = inps;
    fitness = fit;
}

Individual::Individual(Individual &i) {
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
    rand_gen.seed(seed);
}

RandGen::RandGen(unsigned s) {
    seed = s;
    rand_gen.seed(seed);
}

InputState *RandGen::randState() {
    return new InputState {randBool(), randBool(), randBool()};
}

int RandGen::randInt(int min, int max) {
    uniform_int_distribution<std::mt19937::result_type> n(min, max);
    return n(rand_gen);
}

bool RandGen::randBool() {
    return (bool) randInt(0,1);
}
 
unsigned RandGen::genSeed() {
    return 0;
    // return random_device()();
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

GP::GP(int size) {
    pop_size = size;
    pop.reserve(pop_size);
    initPop();
}

GP::~GP() {
    for (Individual *i : pop) {
 	delete i;
    }

    pop.clear();
}

void GP::initPop() {
    for (int i=0; i < pop_size; i++) {
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

    tuple<int,int,int,int> indices = r.randIndices(ind);

    int r1 = get<0>(indices);
    int c1 = get<1>(indices);
    int r2 = get<2>(indices);
    int c2 = get<3>(indices);
    
    for (int i = r1; i < r2; i++) {
	for (int k = c1; k < c2; k++) {
	    ind->inputs[i][k]->mainThruster = r.randBool();
	    ind->inputs[i][k]->rotLeftThruster = r.randBool();
	    ind->inputs[i][k]->rotRightThruster = r.randBool();
	}
    }
}

tuple<Individual *, Individual *> GP::crossover(Individual *i1, Individual *i2) {
    assert(i1->inputs.size() == i2->inputs.size());
    assert(i2->inputs[0].size() == i2->inputs[0].size());

    tuple<Individual *, Individual *> results;
    
    Individual *new_i1 = i1;
    Individual *new_i2 = i2;

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
    
    results = make_tuple(new_i1, new_i2);
    
    return results;
}

void GP::evaluate(Individual *i) {
    GameEngine g;

    g.setDefaultFields();
    g.setBounds(-(HALF_W), HALF_W, SCREEN_HEIGHT);
    
    GameState gs = g.getState();
    
    while (!gs.gameOver) {
	int x = gs.shipXPos + HALF_W;
	int y = gs.shipYPos;
	
	x = floor(x / UNIT_SIZE);
	y = floor(y / UNIT_SIZE);
	
	gs = g.step(*(i->inputs[x][y]));
    }
    
    i -> fitness = gs.score;   
}

bool compareIndividuals(Individual *i1, Individual *i2) {
    return (i1->fitness > i2->fitness);
}


void GP::sortPopulation() {
    std::sort(pop.begin(), pop.end(), compareIndividuals);
}

vector<Individual *> tournamentSelection
