#include "mdmtinstance.h"


#define MAX_GLOBAL_NEIGHBOUR_SEARCHES 2*patience


#define DEBUG
#ifdef DEBUG
#include <iostream>
#define WARN_GLOBAL_SEARCH
#define SHOW_VALUES
#define SHOW_SOLUTION_VECTOR
//#define SHOW_STM
#endif

MDMTInstance::MDMTInstance(const wchar_t* filename, int tenure, int patience, int initialSolutionType){

    std::wifstream inst_stream(filename);
    if(!inst_stream){
        throw std::runtime_error("Could not open file");
    }

    input_filename = filename;
    this->initialSolutionType = initialSolutionType;
    
    inst_stream >> M_size;
    inst_stream >> L_size;
    inst_stream >> l;
    
    solution = new int [L_size];
    distances = new float [M_size * L_size];
    
    for(int i = 0; i < M_size; i++){
        for(int j = 0; j < L_size; j++){
            inst_stream >> distances[i*M_size + j];
        }
    }

    short_term_memory = new int [L_size];
    for(int i = 0; i < L_size; i++){
        short_term_memory[i] = 0;
    }

    this->patience = patience;
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

std::wstring MDMTInstance::getEndingReason(){
    return ending_reason;
}



void MDMTInstance::writeResultsToFile(){
    std::wstring output_filename = input_filename;
    size_t ext_pos = output_filename.find_last_of('.');
    output_filename.replace(ext_pos, output_filename.size(), L".out");
    
    std::wofstream out(output_filename, std::ios_base::app);
    std::wostringstream data;
    data << "INPUT FILE = " << input_filename << '\n';
    data << "TENURE = " << tenure << '\n';
    data << "PATIENCE = " << patience << '\n';
    data << "INITIAL SOLUTION = " << ((initialSolutionType == INITIAL_SOLUTION_STEPS) ? "STEPS" : "RANDOM") << std::endl;
    data << "SEED = " << random_seed << '\n';
    data << "ITERATIONS RAN = " << cur_iteration << '\n';
    data << "REASON = " << ending_reason << '\n';
    data << "TABU RUNTIME = " << tabu_runtime << " SECONDS\n";
    data << "BEST VALUE = " << globalBest << '\n';
    data << "-----------------------------" << '\n';
    out << data.str();
    out.close();

}




// Busca tabu
void MDMTInstance::tabuSearch(int iterations){

    generateInitialSolution();

    int iterations_since_last_improvement = 0;
    bool used_partition = false;
    tabu_runtime = clock() / CLOCKS_PER_SEC;

    for(cur_iteration = 0; cur_iteration < iterations; cur_iteration++){

        // If there's been no changes to the best global value
        if(iterations_since_last_improvement > patience){
            if(iterations_since_last_improvement > MAX_GLOBAL_NEIGHBOUR_SEARCHES){
                ending_reason = L"Too many global neighbourhood searches";
                tabu_runtime = (clock() / CLOCKS_PER_SEC) - tabu_runtime;
                return;
            }
            #ifdef WARN_GLOBAL_SEARCH
            std::cout << "At it=" << cur_iteration << " : Using global neighbourhood" << std::endl;
            #endif
            moveToBestNeighbour();
            used_partition = false;
        }
        else{
            moveToBestNeighbourPartition();
            used_partition = true;
        }

        if(curBest > globalBest){
            globalBest = curBest;
            iterations_since_last_improvement = 0;
        }
        else{
            iterations_since_last_improvement++;
        }
    
        #ifdef SHOW_VALUES
        // if(BEST_VALUE_CHANGED || ((i % 100) == 0)){
        system("cls");
        std::cout << "It " << cur_iteration+1 << " - " << globalBest << "(Cur " << curBest << ')' << std::endl;
        std::cout << "Neighbourhood function: " << (used_partition ? "Partition" : "Global")<< std::endl;
        #endif

        #ifdef SHOW_SOLUTION_VECTOR
        for(int j = 0; j < L_size; j++){
            std::cout << solution[j];
        }
        std::cout << std::endl;
        #endif
        #ifdef SHOW_STM
        std::cout << "Short Term Memory:" << std::endl;
        for(int j = 0; j < short_term_memory_length*2; j+=2){
            std::cout << '|' << short_term_memory[j] << ':' << short_term_memory[j+1];
        }
        #endif

    }
    tabu_runtime = (clock() / CLOCKS_PER_SEC) - tabu_runtime;
    ending_reason = L"Iteration limit reached";
}


// Generates an initial solution
void MDMTInstance::generateInitialSolution(){
    //Fill solution with zeros
    for(int i = 0; i < L_size; i++){
        solution[i] = 0;
    }

    random_seed = time(NULL);
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
}

// Moves to the best neighbour in the partition (best value obtained by moving switching the values of two vertices
// with values 1 and 0 with no ones in between)
void MDMTInstance::moveToBestNeighbourPartition(){
    float aux;
    int old_idx;
    int new_idx = -1;

    curBest = 0;

    int left_idx, right_idx;
    for(int i = 0; i < L_size; i++){
        if(solution[i] != 1){
            continue;
        }

        // Check to the left
        solution[i] = 0;
        for(int j = 1; j < i; j++){
            left_idx = i-j;
            if((left_idx < 0) || (solution[left_idx] == 1)) break;
            
            solution[left_idx] = 1;
            aux = calculateCurrentValue();
            // Aspiration criteria: If this solution is better than the global solution, tabu status is ignored
            if((aux > globalBest) || (!isTabu(i) && aux > curBest)){
                curBest = aux;
                old_idx = i;
                new_idx = left_idx;                
            }

            solution[left_idx] = 0;
        }

        //Check to the right
        for(int j = 1; j < L_size - i; j++){
            right_idx = i+j;
            if((right_idx == L_size) || (solution[right_idx] == 1)) break;
            
            solution[right_idx] = 1;
            aux = calculateCurrentValue();
            // Aspiration criteria: If this solution is better than the global solution, tabu status is ignored
            if((aux > globalBest) || (!isTabu(i) && aux > curBest)){
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
        addToTabu(new_idx);
    }

}



// Moves the solution to the best possible neighbour, considering all possible swaps
void MDMTInstance::moveToBestNeighbour(){
    float aux;
    int old_idx;
    int new_idx = -1;

    curBest = 0;

    int left_idx, right_idx;
    for(int i = 0; i < L_size; i++){
        if(solution[i] != 1){
            continue;
        }

        solution[i] = 0;

        // Check to the left
        solution[i] = 0;
        for(int j = 1; j < i; j++){
            left_idx = i-j;
            if(solution[left_idx] == 1) continue;
            
            solution[left_idx] = 1;
            aux = calculateCurrentValue();
            // Aspiration criteria: If this solution is better than the global solution, tabu status is ignored
            if((aux > globalBest) || (!isTabu(i) && aux > curBest)){
                curBest = aux;
                old_idx = i;
                new_idx = left_idx;                
            }

            solution[left_idx] = 0;
        }

        //Check to the right
        for(int j = 1; j < L_size - i; j++){
            right_idx = i+j;
            if(right_idx == L_size) break;
            if(solution[right_idx] == 1) continue;
            
            solution[right_idx] = 1;
            aux = calculateCurrentValue();
            // Aspiration criteria: If this solution is better than the global solution, tabu status is ignored
            if((aux > globalBest) || (!isTabu(i) && aux > curBest)){
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
        addToTabu(new_idx);
    }

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
    return short_term_memory[vertix_idx] < cur_iteration;
}


// Set the iteration upon which this vertix will be considered non-tabu
 void MDMTInstance::addToTabu(int vertix_idx){
    short_term_memory[vertix_idx] = cur_iteration + tenure;
}