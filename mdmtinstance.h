#ifndef MDMTINSTANCE_H
#define MDMTINSTANCE_H
#include <fstream>

class MDMTInstance{

    private:
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
    MDMTInstance(wchar_t* filename, int tenure, int patience);
    ~MDMTInstance();

    // Getters
    int getM_size();
    int getL_size();
    int getl_size();
    float getglobalBest();

    void tabuSearch(int iterations);

};

#endif