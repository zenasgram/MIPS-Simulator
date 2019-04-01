#ifndef PROCESSOR_HPP
#define PROCESSOR_HPP

#include <cstdint>
#include "instructions/bitwise.hpp"
#include "instructions/arithmetic.hpp"
#include "register_file.hpp"
#include "program_counter.hpp"

typedef uint32_t operand;
typedef uint32_t memory_address;

class processor {
    private:
        operand hi_register;
        operand lo_register;
    public:
        operand execute_r_instruction(operand funct, operand rs_val, operand rt_val, operand shamt);
        uint32_t sign_extend8(uint32_t data);
        uint32_t sign_extend16(uint32_t data);
        uint32_t sign_extend18(uint32_t data);
        bool greater_than_zero(operand data);
        bool equal_zero(operand data);
        bool less_than_zero(operand data);

};
#endif
