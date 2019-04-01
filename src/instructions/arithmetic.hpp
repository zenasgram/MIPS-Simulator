#ifndef ARITHMETIC_HPP
#define ARITHMETIC_HPP

#include <cstdint>

typedef uint32_t operand;
class arithmetic {
    private:
        bool zero;
    public:
        operand add_unsigned(operand x, operand y);
        operand sub_unsigned(operand x, operand y);
        operand set_less_than_unsigned(operand x, operand y);
        long long multiply_unsigned(operand x, operand y);
        operand division_quotient(operand x, operand y);
        operand division_remainder(operand x, operand y);
        operand invert(operand x);
};


#endif
