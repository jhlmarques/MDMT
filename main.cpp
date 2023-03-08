#include <iostream>
#include "mdmtinstance.h"

int wmain(int argc, wchar_t* argv[]) {
    
    if(argc < 2){
        std::cout << "Usage: tabusearch <path-to-instance>"<< std::endl;
        return -1;
    }

    wchar_t* filename = argv[1];
    
    try{
        MDMTInstance instance(filename);
        std::cout << instance.getM_size() << std::endl;
        std::cout << instance.getL_size() << std::endl;
        std::cout << instance.getl_size() << std::endl;
        instance.tabuSearch(10, 5, 10);
        std::cout << "Best value: " << instance.getBestValue();
    }
    catch (std::exception& e){
        std::cerr << e.what();
        return -1;
    }



}
