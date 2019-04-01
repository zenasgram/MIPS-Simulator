#include "program_counter.hpp"


program_counter::program_counter() {
    address = 0x10000000;
    memory_base_address = 0x10000000;
    memory_size = 0x1000000;
}

program_counter::program_counter(word initial_address, word memory_base_address_in, word memory_size_in) {
    address = initial_address;
    memory_base_address = memory_base_address_in;
    memory_size = memory_size_in;
}

void program_counter::check_valid_address() {
    if (address < memory_base_address || address > (memory_base_address+memory_size)) {
        std::exit(-11);
    }
}

word program_counter::get_current_address() const {
    return address;
}

word program_counter::get_memory_base_address() const {
    return memory_base_address;
}

word program_counter::get_memory_size() const {
    return memory_size;
}

word program_counter::increment() {
    address += 4;
    check_valid_address();
    return address;
}

word program_counter::jump_offset(word offset) {
    address += offset;
    check_valid_address();
    return address;
}

word program_counter::set_address(word address_in) {
    check_valid_address();
    address = address_in;
    return address;
}
