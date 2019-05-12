#include "gp.h"

using namespace std;


int main () {
    assert(SCREEN_WIDTH % UNIT_SIZE == 0);
    assert(SCREEN_HEIGHT % UNIT_SIZE == 0);    
	
    GP gp(POP_SIZE);

    for (Individual *i : gp.pop) {
	GameEngine g;
	g.setDefaultFields();
	g.setBounds(-(HALF_W), HALF_W, SCREEN_HEIGHT);
	
	GameState gs {0,0,0,0,0,0,0,0,0}; // Problem
	
	while (!gs.gameOver) {
	    int x = gs.shipXPos + HALF_W;
	    int y = gs.shipYPos;
	    	    
	    x = floor(x / UNIT_SIZE);
	    y = floor(y / UNIT_SIZE);

	    gs = g.step(*(i->inputs[x][y]));
	}
	
	i -> fitness = gs.score;

	i -> print();
    }
    
    return 0;
}


Individual::Individual(vector<vector<InputState *>> inps, double fit) {
    inputs = inps;
    fitness = fit;
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
    return random_device()();
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
    int max_r = ind->inputs.size();
    int max_c = ind->inputs[0].size();

    int r1 = r.randInt(0, max_r);
    int c1 = r.randInt(0, max_c);
    
    int r2 = r.randInt(r1, max_r);
    int c2 = r.randInt(c1, max_c);
    
    for (int i = r1; i < r2; i++) {
	for (int k = c1; k < c2; k++) {    
	    ind->inputs[i][k]->mainThruster = r.randBool();
	    ind->inputs[i][k]->rotLeftThruster = r.randBool();
	    ind->inputs[i][k]->rotRightThruster = r.randBool();
	}
    }
}

tuple<Individual *, Individual *> GP::crossover(Individual *i1, Individual *i2) {
    tuple <Individual *, Individual *> results;
    
    Individual *new_i1 = new Individual {i1->inputs, i1->fitness};
    Individual *new_i2 = new Individual {i2->inputs, i2->fitness};
    
    int r1 = r.randInt(0, VECT_H);
    int c1 = r.randInt(0, VECT_W);
    
    int r2 = r.randInt(r1, VECT_W);
    int c2 = r.randInt(c1, VECT_H);
    
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

