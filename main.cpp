#include <iostream>
#include <fstream>

// Obtém informações relevantes a uma instância do problema MDMT
void parseMDMTInstance(wchar_t* filename, int& M_size, int& L_size, int& l, float* distances){

    std::wifstream inst_stream(filename);
    if(!inst_stream){
        throw std::runtime_error("Could not open file");
    }


    inst_stream >> M_size;
    inst_stream >> L_size;
    inst_stream >> l;
    
    distances = new float [M_size * L_size];

    for(int i = 0; i < M_size; i++){
        for(int j = 0; j < L_size; j++){
            inst_stream >> distances[i*M_size + j];
        }
    }

}

int wmain(int argc, wchar_t* argv[]) {
    
    if(argc < 2){
        std::cout << "Usage: tabusearch <path-to-instance>"<< std::endl;
        return -1;
    }

    wchar_t* filename = argv[1];
    
    
    int M_size, L_size, l;
    float* distances = NULL;

    try{
        parseMDMTInstance(filename, M_size, L_size, l, distances);
    }
    catch (std::exception& e){
        std::cerr << e.what();
        return -1;
    }

    std::cout << M_size << std::endl;
    std::cout << L_size << std::endl;
    std::cout << l << std::endl;




}
