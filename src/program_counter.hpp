#ifndef PROGRAM_COUNTER_HPP
#define PROGRAM_COUNTER_HPP

#include <cstdint>
#include <cstdlib>

typedef uint32_t word;

class program_counter {
    private:
        word address;
        word memory_base_address;
        word memory_size;
        void check_valid_address();
    public:
        program_counter();
        program_counter(word initial_address, word memory_base_address_in = 0x10000000, word memory_size_in = 0x1000000);
        word get_current_address() const;
        word get_memory_base_address() const;
        word get_memory_size() const;
        word increment();
        word jump_offset(word offset);
        word set_address(word address_in);
};
#endif
