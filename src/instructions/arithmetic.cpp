#include "arithmetic.hpp"


operand arithmetic::add_unsigned(operand x, operand y) {
    return x+y;
}

operand arithmetic::sub_unsigned(operand x, operand y) {
    return x-y;
}

operand arithmetic::set_less_than_unsigned(operand x, operand y) {
    if( x < y ){
        return 1;
    }
    else{
        return 0;
    }
}

long long arithmetic::multiply_unsigned(operand x, operand y) {
    return (long long)x * (long long) y;
}

operand arithmetic::division_quotient(operand x, operand y) {
    return x/y;
}
operand arithmetic::division_remainder(operand x, operand y) {
    return x%y;
}

operand arithmetic::invert(operand x) {
    return (~x)+1;
}
