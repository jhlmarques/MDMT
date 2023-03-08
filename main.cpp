#include <iostream>
#include "mdmtinstance.h"

int wmain(int argc, wchar_t* argv[]) {
    
    if(argc < 5){
        std::cout << "Usage: tabusearch <path-to-instance> <iterations> <tenure> <memory>"<< std::endl;
        return -1;
    }

    wchar_t* filename = argv[1];
    int iterations = _wtoi(argv[2]);
    int tabu_tenure = _wtoi(argv[3]);
    int memory_size = _wtoi(argv[4]);
     
    try{
        MDMTInstance instance(filename);
        std::cout << "Instance info:" << std::endl;
        std::cout << instance.getM_size() << ' ';
        std::cout << instance.getL_size() << ' ';
        std::cout << instance.getl_size() << std::endl;
        std::cout << "--------------------------" << std::endl;
        instance.tabuSearch(iterations, tabu_tenure, memory_size);
        std::cout << "Best value: " << instance.getBestValue();
    }
    catch (std::exception& e){
        std::cerr << e.what();
        return -1;
    }



}
