#include <iostream>
#include <iomanip>
#include <ctime>
#include "mdmtinstance.h"

int main(int argc, char* argv[]) {
    
    if(argc != 7){
        std::cout << "Usage: tabusearch <result-file> <iterations> <tenure> <solution_type> <random_seed> <use_aspiration_criteria>"<< std::endl;
        return -1;
    }

    char* filename = argv[1];
    int iterations = atoi(argv[2]);
    int tabu_tenure = atoi(argv[3]);
    int initial_solution_type = atoi(argv[4]) == 1 ? INITIAL_SOLUTION_RANDOM : INITIAL_SOLUTION_STEPS;
    int random_seed = atoi(argv[5]);
    int use_aspiration_criteria = atoi(argv[6]);
     
    try{

        MDMTInstance instance(filename, tabu_tenure, initial_solution_type, random_seed, use_aspiration_criteria);
        std::cout << "Instance info:" << std::endl;
        std::cout << "|M| = " << instance.getM_size() << ' ';
        std::cout << "|L| = " <<instance.getL_size() << ' ';
        std::cout << "l = " << instance.getl_size() << std::endl;
        std::cout << "Running..." << std::endl;
        instance.tabuSearch(iterations);
        std::cout << "Best value: " << instance.getglobalBest() << std::endl;
        instance.writeResultsToFile();

    }
    catch (std::exception& e){
        std::cerr << e.what();
        return -1;
    }



}
