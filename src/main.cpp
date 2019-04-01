#include <iostream>
#include <fstream>
#include <cstring>
#include <vector>
#include <string>
#include "simulator.hpp"

typedef char type;

int main(int argc, char* argv[]) {

	// std::cerr << "\n=============================" << std::endl;
    // std::cerr << "        MIPS SIMULATOR       " << std::endl;
    // std::cerr << "=============================" << std::endl;
    std::string mips_bin;
	if (argc>1) {
		mips_bin = argv[1];
	}
	else {
		std::cerr << "No input binary file specified" << std::endl;
		std::exit(-20);
	}



//*******************************************************************************
//EXTRACTING BINARIES from BIN FILE --------- file passed via COMMAND LINE PARAMETER

    std::ifstream is (mips_bin, std::ifstream::binary);
        if (is) {
            // get length of file:
            is.seekg (0, is.end);
            int length = is.tellg();
            is.seekg (0, is.beg);
            std::vector<type> binaries(length,0);
            // std::cerr << "\nBinary file contains " << length << " characters \n" << std::endl;

            int size = sizeof(unsigned char);
             for(int i=0; i<length; i++){
                // read data as a block:
                is.read (&binaries[i], size);
                }

//*******************************************************************************
//LOADING BINARIES into INSTRUCTION MEM

            simulator vsim(binaries);
            vsim.start(0x10000000);
			int status = 0;
			int i = 1;
            while (i != 0) {
				status = vsim.step();
                if (status == 1) {
				    int d;
				    char c;
				    while ((d = getchar()) != '\n') {
				        c = (char)d;
				    }
					vsim.store_input(c);
				}
            }
        }
        else {
            std::cerr << "Unable to open binary file." << std::endl;
			std::exit(-21);
        }
    return 0;
}
