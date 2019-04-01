#ifndef SIMULATOR_HPP
#define SIMULATOR_HPP

#include <string>
#include <cstdint>
#include <vector>
#include "program_counter.hpp"
#include "processor.hpp"
#include "register_file.hpp"
#include "instruction_memory.hpp"

typedef uint32_t mem_address;
typedef uint32_t instruction;

class simulator{
    private:
        program_counter pc;
        processor ip;
        register_file rf;
        instruction_memory im;

    public:
        simulator(std::vector<char> binary);
        void start(mem_address memory_address);
        int step();
        instruction fetch_instruction();
        int decode_and_execute_instruction(instruction inst);
        void execute_delayed_slot();
        void store_input(char input);
};
#endif
