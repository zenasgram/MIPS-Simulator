#ifndef BITWISE_HPP
#define BITWISE_HPP

#include <cstdint>

typedef uint32_t operand;
class bitwise {
private:
public:
    operand bitwise_or(operand x, operand y);
    operand bitwise_xor(operand x, operand y);
    operand bitwise_and(operand x, operand y);
    operand bitwise_nor(operand x, operand y);
    operand bitwise_sll(operand x, operand y);
    operand bitwise_srl(operand x, operand y);
    operand bitwise_sra(operand x, operand y);
};
#endif
