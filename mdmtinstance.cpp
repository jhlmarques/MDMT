#include "mdmtinstance.h"
#include <iostream>
#include <random>
#include <time.h>

#define DEBUG

#ifdef DEBUG
int CURRENT_ITERATION;
bool BEST_VALUE_CHANGED;
#endif

MDMTInstance::MDMTInstance(wchar_t* filename){

    std::wifstream inst_stream(filename);
    if(!inst_stream){
        throw std::runtime_error("Could not open file");
    }

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

}

MDMTInstance::~MDMTInstance(){
    delete[] solution;
    delete[] distances;
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

float MDMTInstance::getBestValue(){
    return bestValue;
}


// Busca tabu
int MDMTInstance::tabuSearch(int iterations, int tabu_tenure, int sm_size){

    //Index i: index of a vertix
    //Index i+1: tabu tenure of said vertix
    short_term_memory = new int [sm_size*2];
    short_term_memory_length = sm_size;
    for(int i = 0; i < sm_size * 2; i++){
        short_term_memory[i] = 0;
    }


    //Fill solution with zeros
    for(int i = 0; i < L_size; i++){
        solution[i] = 0;
    }
    
    srand(time(NULL));

    // //Generate random solution
    // int random_assignments = l;
    // int random_idx;
    // while(random_assignments > 0){
    //     random_idx = rand() % L_size;
    //     if(solution[random_idx] == 0){
    //         solution[random_idx] = 1;
    //         random_assignments--;
    //     }
    // }
    //Set an element to one every x steps
    int step = (int) floor(L_size / l); 
    int elements_added = 0;
    std::cout << step << std::endl;
    for(int i = 0; i < (L_size - L_size%4); i+= step){
        elements_added++;
        solution[i] = 1;
    }
    if(elements_added != l){
        std::cout << elements_added << std::endl;
        throw std::runtime_error("Something went wrong...");
    }

    #ifdef DEBUG
    for(int i = 0; i < L_size; i++){
        std::cout << solution[i] << ' ';
    }
    std::cout << std::endl;
    #endif

    int moved_idx;

    for(int i = 0; i < iterations; i++){

        #ifdef DEBUG
        CURRENT_ITERATION = i;
        #endif

        updateTabu();
        moved_idx = moveToBestNeighbour();
        // If moved to a neighbour, add the moved vertix idx to tabu
        if(moved_idx > -1){
            addToTabu(moved_idx, tabu_tenure);
        }
        else{
            std::cout << i << " : No movement!" << std::endl;
        }
    
        #ifdef DEBUG
        // if(BEST_VALUE_CHANGED || ((i % 100) == 0)){
            system("cls");
            std::cout << "It " << i << " - " << bestValue << std::endl;
            for(int j = 0; j < L_size; j++){
                std::cout << solution[j];
            }
            std::cout << std::endl;
            std::cout << "Short Term Memory:" << std::endl;
            for(int j = 0; j < short_term_memory_length*2; j+=2){
                std::cout << '|' << short_term_memory[j] << ':' << short_term_memory[j+1];
            }


        // }
        #endif

    }

    std::cout << "End!" << std::endl;
    delete[] short_term_memory;

    return 0;
}

// Moves the solution to the best possible neighbour. Returns the index of the latest
// changed vertix
 int MDMTInstance::moveToBestNeighbour(){
    #ifdef DEBUG
    BEST_VALUE_CHANGED = false;
    #endif

    float best = -1;
    int best_idx;
    int best_movement;

    float aux;

    for(int i = 0; i < L_size; i++){
        if(solution[i] != 1){
            continue;
        }

        for(int j = 1; j < L_size; j++){
            //Try moving to the left
            int left_i;
            if(j > i){
                left_i = L_size - (j-i);
            }
            else{
                left_i = i-j;
            }
            
            if(solution[left_i] == 1){
                break;   
            }
            else{
                solution[left_i] = 1;
                solution[i] = 0;
                //Get the function value for this selection
                aux = calculateCurrentValue();
                //Aspiration criteria: ignore tabu if a better value was found
                if(aux > bestValue){
                    best = aux;
                    bestValue = aux;
                    best_idx = i;
                    best_movement = -j;

                    #ifdef DEBUG
                    BEST_VALUE_CHANGED = true;
                    #endif

                }
                else{
                    //Check if movement to this vertix is tabu
                    if(!isInTabu(left_i)){
                        //If not, check if this neighbour is better than
                        //the others
                        if(aux > best){
                            best = aux;
                            best_idx = i;
                            best_movement = -j;
                        }
                    }
                }

                //Undo move
                solution[left_i] = 0;
                solution[i] = 1;
            }
        }
        for(int j = 1; j < L_size; j++){
            //Try moving to the right
            int right_i = j+i;
            if(right_i >= L_size){
                right_i = right_i - L_size;
            }
            
            if(solution[right_i] == 1){
                break;   
            }
            else{
                solution[right_i] = 1;
                solution[i] = 0;
                //Get the function value for this selection
                aux = calculateCurrentValue();
                //Aspiration criteria: ignore tabu if a better value was found
                if(aux > bestValue){
                    best = aux;
                    bestValue = aux;
                    best_idx = i;
                    best_movement = j;

                    #ifdef DEBUG
                    BEST_VALUE_CHANGED = true;
                    #endif

                }
                else{
                    //Check if movement to this vertix is tabu
                    if(!isInTabu(right_i)){
                        //If not, check if this neighbour is better than
                        //the others
                        if(aux > best){
                            best = aux;
                            best_idx = i;
                            best_movement = j;
                        }
                    }
                }

                //Undo move
                solution[right_i] = 0;
                solution[i] = 1;
            }
        }
    }

    // If there's no avaiable neighbour, return a special code (-1)
    if(best == -1){
        return -1;
    }

    // Move to the best neighbour
    solution[best_idx + best_movement] = 1;
    solution[best_idx] = 0;

    return best_idx + best_movement;

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
inline int MDMTInstance::isInTabu(int vertix_idx){
    for(int i = 0; i < short_term_memory_length*2; i+=2){
        if(short_term_memory[i] == vertix_idx){
            return true;
        }
    }
    return false;
}

// Put this vertix in tabu, overwriting the oldest tabu vertix
inline void MDMTInstance::addToTabu(int vertix_idx, int tenure){
    short_term_memory[stm_oldest*2] = vertix_idx;
    short_term_memory[stm_oldest*2 + 1] = tenure;
    stm_oldest = (stm_oldest+1 > short_term_memory_length) ? 0 : stm_oldest+1;
}

// Update the tabu status of the vertices in the tabu list
 void MDMTInstance::updateTabu(){
    for(int i = 0; i+1 < 2*short_term_memory_length; i+=2){
        if(short_term_memory[i+1] > 0){
            short_term_memory[i+1]--;
        }
        else{
            short_term_memory[i] = -1;
        }
    }
}