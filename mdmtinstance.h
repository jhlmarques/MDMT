#ifndef MDMTINSTANCE_H
#define MDMTINSTANCE_H
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <random>
#include <time.h>

#define INITIAL_SOLUTION_STEPS 0
#define INITIAL_SOLUTION_RANDOM 1

class MDMTInstance{

    private:
    std::string output_filename;
    int initialSolutionType;
    time_t random_seed;
    float tabu_runtime = 0;

    int M_size;
    int L_size;
    int l;

    float* distances;
    int* solution;

    float globalBest = 0;
    float curBest = 0;
    int cur_iteration = 0;

    int* short_term_memory;
    
    // Tabu search parameters
    int tenure;

    // Essential functions for tabu search
    void generateInitialSolution();
    void moveToBestNeighbour();
    void moveToBestNeighbourPartition();
    float calculateCurrentValue();
    bool isTabu(int vertix_idx);
    void addToTabu(int vertix_idx);

    public:
    MDMTInstance(const char* output_filename, int tenure, int initialSolutionType);
    ~MDMTInstance();
    
    void writeResultsToFile();

    // Getters
    int getM_size();
    int getL_size();
    int getl_size();
    float getglobalBest();
    

    void tabuSearch(int iterations);

};

#endif