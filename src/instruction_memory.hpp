#ifndef INSTRUCTION_MEMORY_HPP
#define INSTRUCTION_MEMORY_HPP

#include <iostream>
#include <vector>
#include <sstream>
#include <fstream>
#include <vector>
#include <string>

typedef char type;

class instruction_memory {
    private:
        uint32_t offset;
        uint32_t length;
        std::vector<type> insmem;
        bool getc_wait;
    public:
        //default constructor
        instruction_memory();
        instruction_memory(uint32_t offset, uint32_t length);

        uint32_t get_offset() const;
        uint32_t get_length() const;

        void set_offset (uint32_t offset_in);
        void set_length (uint32_t length_in);

        void init (std::vector<type>& binaries);
        void load_instr(std::vector<type>& binaries);

        uint32_t fetch_instruction(uint32_t PC);

        char addr_getc();
        void addr_putc();
        uint32_t get_word(uint32_t address);
        void put_word(uint32_t address, uint32_t data);

        int load_word(uint32_t address, uint32_t& data);
        int load_byte(uint32_t address, char& data);
        void store_word(uint32_t address, uint32_t data);
        void store_byte(uint32_t address, char data);

        void store_half(uint32_t address, uint32_t data);
};
#endif
