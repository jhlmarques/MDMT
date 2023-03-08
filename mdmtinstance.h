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
    float bestValue = 0;
    int* short_term_memory;
    int short_term_memory_length;
    // Index of the oldest element of the short term memory
    int stm_oldest = 0;

    // Essential functions for tabu search
    inline int moveToBestNeighbour();
    inline float calculateCurrentValue();
    inline int isInTabu(int vertix_idx);
    inline void addToTabu(int vertix_idx, int tenure);
    inline void updateTabu();

    public:
    MDMTInstance(wchar_t* filename);
    ~MDMTInstance();

    // Getters
    int getM_size();
    int getL_size();
    int getl_size();
    float getBestValue();

    int tabuSearch(int iterations, int tabu_tenure, int sm_size);
};

// class MDMTSolution{
//     int* solution;

//     MDMTSolution(size_t size);
//     ~MDMTSolution();

//     public:
//     void updateCurrentNeighbours();


// }

#endif