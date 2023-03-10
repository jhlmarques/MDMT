#include <iostream>
#include <string>
#include "mdmtinstance.h"

int wmain(int argc, wchar_t* argv[]) {
    
    if(argc < 6){
        std::cout << "Usage: tabusearch <path-to-instance> <iterations> <tenure> <patience> <solution_type>"<< std::endl;
        return -1;
    }

    wchar_t* filename = argv[1];
    int iterations = _wtoi(argv[2]);
    int tabu_tenure = _wtoi(argv[3]);
    int patience = _wtoi(argv[4]);
    int initial_solution_type = _wtoi(argv[5]) == 1 ? INITIAL_SOLUTION_RANDOM : INITIAL_SOLUTION_STEPS;
     
    try{
        MDMTInstance instance(filename, tabu_tenure, patience, initial_solution_type);
        std::cout << "Instance info:" << std::endl;
        std::cout << "|M| = " << instance.getM_size() << ' ';
        std::cout << "|L| = " <<instance.getL_size() << ' ';
        std::cout << "l = " << instance.getl_size() << std::endl;
        std::cout << "Running..." << std::endl;
        instance.tabuSearch(iterations);
        std::wcout << L"The search has stopped. Reason: " << instance.getEndingReason() << std::endl;
        std::cout << "Best value: " << instance.getglobalBest() << std::endl;
        instance.writeResultsToFile();

    }
    catch (std::exception& e){
        std::cerr << e.what();
        return -1;
    }



}
