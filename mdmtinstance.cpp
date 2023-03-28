#include "mdmtinstance.h"

//#define DEBUG_PRINTING
#ifdef DEBUG_PRINTING
#include <iostream>
#define WARN_GLOBAL_SEARCH
#define SHOW_VALUES
#define SHOW_SOLUTION_VECTOR
int switched_new, switched_old;

#endif

MDMTInstance::MDMTInstance(const char* output_filename, int tenure, int initialSolutionType, int random_seed, int use_aspiration_criteria){

    this->output_filename = output_filename;
    this->initialSolutionType = initialSolutionType;
    this->random_seed = random_seed;
    this->use_aspiration_criteria = use_aspiration_criteria;
    
    std::cin >> M_size;
    std::cin >> L_size;
    std::cin >> l;
    
    solution = new int [L_size];
    distances = new float [M_size * L_size];
    
    for(int i = 0; i < M_size; i++){
        for(int j = 0; j < L_size; j++){
            std::cin >> distances[i*M_size + j];
        }
    }

    short_term_memory = new int [L_size];
    for(int i = 0; i < L_size; i++){
        short_term_memory[i] = 0;
    }

    this->tenure = tenure;


}

MDMTInstance::~MDMTInstance(){
    delete[] solution;
    delete[] distances;
    delete[] short_term_memory;
}

int MDMTInstance::getM_size(){
    return M_size;
}

int MDMTInstance::getL_size(){
    return L_size;
}

int MDMTInstance::getl_size(){
    return l;
}

float MDMTInstance::getglobalBest(){
    return globalBest;
}




void MDMTInstance::writeResultsToFile(){
    
    std::ofstream out(output_filename, std::ios_base::app);
    std::ostringstream data;
    data << "RESULTS FOR INSTANCE WITH" << " |M|=" << getM_size() << " |L|=" << getL_size() << " l=" << getl_size() << std::endl;
    data << "TENURE = " << tenure << '\n';
    data << "INITIAL SOLUTION GENERATION: = " << ((initialSolutionType == INITIAL_SOLUTION_STEPS) ? "STEPS" : "RANDOM") << std::endl;
    data << "RANDOM SEED = " << random_seed << '\n';
    data << "FACTORED ASPIRATION CRITERIA = " << use_aspiration_criteria << '\n';
    data << "ITERATIONS RAN = " << cur_iteration << '\n';
    data << "TABU RUNTIME = " << tabu_runtime << " SECONDS\n";
    data << "BEST KNOWN VALUE = " << globalBest << '\n';
    data << "-----------------------------" << '\n';
    out << data.str();
    out.close();

}




// Busca tabu
void MDMTInstance::tabuSearch(int iterations){

    generateInitialSolution();

    tabu_runtime = clock() / CLOCKS_PER_SEC;

    for(cur_iteration = 0; cur_iteration < iterations; cur_iteration++){

        moveToBestNeighbourPartition();

        #ifdef SHOW_VALUES
        system("cls");
        std::cout << "It " << cur_iteration+1 << " - " << globalBest << "(Cur " << curBest << ')' << std::endl;
        #endif

        #ifdef SHOW_SOLUTION_VECTOR
        for(int j = 0; j < L_size; j++){
            if(j == switched_new){
                std::cout << "\033[1;31;40mO\033[0m";
            }
            else if(j == switched_old){
                std::cout << "\033[1;31;40mX\033[0m";
            }
            else{
                std::cout << solution[j];
            }
        }
        std::cout << std::endl;
        #endif

    }
    tabu_runtime = (clock() / CLOCKS_PER_SEC) - tabu_runtime;
}

// Checks if the current solution meets the aspiration criteria using its value
// Aspiration criteria: If this solution is better than the global solution, tabu status is ignored
bool MDMTInstance::checkAspirationCriteria(int value){
    if(use_aspiration_criteria) return value > globalBest;
    return false;
}


// Generates an initial solution
void MDMTInstance::generateInitialSolution(){
    //Fill solution with zeros
    for(int i = 0; i < L_size; i++){
        solution[i] = 0;
    }

    srand(random_seed);

    if(initialSolutionType == INITIAL_SOLUTION_RANDOM){
        //Generate random solution
        int random_assignments = l;
        int random_idx;
        while(random_assignments > 0){
            random_idx = rand() % L_size;
            if(solution[random_idx] == 0){
                solution[random_idx] = 1;
                random_assignments--;
            }
        }
    }

    if(initialSolutionType == INITIAL_SOLUTION_STEPS){
        //Set an element to one every x steps
        int step = (int) floor(L_size / l);
        for(int i = 0; i < (L_size - (L_size % step)); i+= step){
            solution[i] = 1;
        }
    }

    std::cout << "Initial solution value: " << calculateCurrentValue() << std::endl;

}

// Moves to the best neighbour in the partition (best value obtained by moving switching the values of two vertices
// with values 1 and 0 with no ones in between)
void MDMTInstance::moveToBestNeighbourPartition(){
    float aux;
    int old_idx = -1;
    int new_idx = -1;

    curBest = 0;

    int left_idx, right_idx;
    for(int i = 0; i < L_size; i++){

        if(solution[i] != 1){
            continue;
        }

        // Check to the left
        solution[i] = 0;
        for(int j = 1; j < L_size; j++){
            left_idx = i-j;
            if(left_idx < 0) left_idx = L_size + left_idx; 

            if(solution[left_idx] == 1) {break;}
            
            solution[left_idx] = 1;
            aux = calculateCurrentValue();

            if(checkAspirationCriteria(aux) || (!isTabu(i) && aux > curBest)){
                curBest = aux;
                old_idx = i;
                new_idx = left_idx;  
            }

            solution[left_idx] = 0;
        }

        //Check to the right
        for(int j = 1; j < L_size; j++){
            right_idx = i+j;
            if(right_idx >= L_size) right_idx -= L_size;

            if(solution[right_idx] == 1) {break;}
            
            solution[right_idx] = 1;
            aux = calculateCurrentValue();

            if(checkAspirationCriteria(aux) || (!isTabu(i) && aux > curBest)){
                curBest = aux;
                old_idx = i;
                new_idx = right_idx;                  
            }

            solution[right_idx] = 0;
        }
        solution[i] = 1;
    }

    // Move to the best neighbour
    if(new_idx > -1){
        solution[new_idx] = 1;
        solution[old_idx] = 0;
        addToTabu(old_idx);
    }

    #ifdef DEBUG_PRINTING
    switched_new = new_idx;
    switched_old = old_idx;
    #endif

    if(curBest > globalBest){
        globalBest = curBest;
    }


}



// Moves the solution to the best possible neighbour, considering all possible swaps
void MDMTInstance::moveToBestNeighbour(){
    float aux;
    int old_idx = -1;
    int new_idx = -1;

    curBest = 0;

    for(int i = 0; i < L_size; i++){
        if(solution[i] != 1){
            continue;
        }

        solution[i] = 0;

        for(int j = 0; j < L_size; j++){
            if((j == i) || (solution[j] == 1)) continue;

            solution[j] = 1;
            aux = calculateCurrentValue();
            if(aux > curBest){
                curBest = aux;
                old_idx = i;
                new_idx = j;                  
            }
            solution[j] = 0;
        }

        solution[i] = 1;
    }

    // Move to the best neighbour
    if(new_idx > -1){
        solution[new_idx] = 1;
        solution[old_idx] = 0;
        addToTabu(new_idx);
        addToTabu(old_idx);
    }

    #ifdef DEBUG_PRINTING
    switched_new = new_idx;
    switched_old = old_idx;
    #endif

}



// Calculate the value of the current instance
 float MDMTInstance::calculateCurrentValue(){
    float sum_of_min_dists = 0;
    for(int i = 0; i < M_size; i++){
        float min_dist = INFINITY;
        for(int j = 0; j < L_size; j++){
            if(solution[j] == 0){
                continue;
            }

            if(distances[i*M_size + j] < min_dist){
                min_dist = distances[i*M_size + j];
            }

        }
        sum_of_min_dists += min_dist;
    }
    return sum_of_min_dists;
}



// Checks if this vertix is tabu
 bool MDMTInstance::isTabu(int vertix_idx){
    return short_term_memory[vertix_idx] > cur_iteration;
}


// Set the iteration upon which this vertix will be considered non-tabu
 void MDMTInstance::addToTabu(int vertix_idx){
    short_term_memory[vertix_idx] = cur_iteration + tenure;
}