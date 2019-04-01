#include "bitwise.hpp"


operand bitwise::bitwise_or(operand x, operand y){
    return x|y;
}

operand bitwise::bitwise_and(operand x, operand y){
    return x&y;
}

operand bitwise::bitwise_xor(operand x, operand y){
    return x^y;
}

operand bitwise::bitwise_nor(operand x, operand y){
    return ~(x|y);
}

operand bitwise::bitwise_sll(operand x, operand y){
    return x<<y;
}

operand bitwise::bitwise_srl(operand x, operand y){
    return x>>y;
}
operand bitwise::bitwise_sra(operand x, operand y){
    
    operand mask = 0xFFFFFFFF;

    //check MSB; 0 or 1 , eg. y=4
    if((x>>31)==0){
        return x>>y;
    }
    else{
        mask = mask << (32-y); //0xF0000000 
        x = (x>>y) | mask;
        return x;
    }
}

