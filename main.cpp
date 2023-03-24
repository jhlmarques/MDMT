#include <iostream>
#include <iomanip>
#include <ctime>
#include "mdmtinstance.h"

int wmain(int argc, wchar_t* argv[]) {
    
    if(argc < 5){
        std::cout << "Usage: tabusearch <path-to-instance> <iterations> <tenure> <solution_type>"<< std::endl;
        return -1;
    }

    wchar_t* filename = argv[1];
    int iterations = _wtoi(argv[2]);
    int tabu_tenure = _wtoi(argv[3]);
    int initial_solution_type = _wtoi(argv[4]) == 1 ? INITIAL_SOLUTION_RANDOM : INITIAL_SOLUTION_STEPS;
     
    try{
        time_t t;
        tm * cur_time ;

        MDMTInstance instance(filename, tabu_tenure, initial_solution_type);
        std::cout << "Instance info:" << std::endl;
        std::cout << "|M| = " << instance.getM_size() << ' ';
        std::cout << "|L| = " <<instance.getL_size() << ' ';
        std::cout << "l = " << instance.getl_size() << std::endl;
        t = time(nullptr);
        cur_time = localtime(&t);
        std::cout << asctime(cur_time) << " Running..." << std::endl;
        instance.tabuSearch(iterations);
        t = time(nullptr);
        cur_time = localtime(&t);
        std::wcout << asctime(cur_time) << L" The search has stopped." << std::endl;
        std::cout << "Best value: " << instance.getglobalBest() << std::endl;
        instance.writeResultsToFile();

    }
    catch (std::exception& e){
        std::cerr << e.what();
        return -1;
    }



}
