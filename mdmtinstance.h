#ifndef MDMTINSTANCE_H
#define MDMTINSTANCE_H
#include <fstream>
#include <string>
#include <sstream>
#include <random>
#include <time.h>

#define INITIAL_SOLUTION_STEPS 0
#define INITIAL_SOLUTION_RANDOM 1

class MDMTInstance{

    private:
    const wchar_t* input_filename;
    int initialSolutionType;
    time_t random_seed;
    float tabu_runtime = 0;
    std::wstring ending_reason;

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
    int patience;

    // Essential functions for tabu search
    void generateInitialSolution();
    void moveToBestNeighbour();
    void moveToBestNeighbourPartition();
    float calculateCurrentValue();
    bool isTabu(int vertix_idx);
    void addToTabu(int vertix_idx);

    public:
    MDMTInstance(const wchar_t* filename, int tenure, int patience, int initialSolutionType);
    ~MDMTInstance();
    
    void writeResultsToFile();

    // Getters
    int getM_size();
    int getL_size();
    int getl_size();
    float getglobalBest();
    std::wstring getEndingReason();
    


    void tabuSearch(int iterations);

};

#endif