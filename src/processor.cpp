#include "processor.hpp"
#include <iostream>

operand processor::execute_r_instruction(operand funct, operand rs_val, operand rt_val, operand shamt){
    operand result;

    switch (funct) {
        case 0x00: { //sll: logical shift left
            bitwise bw;
            result = bw.bitwise_sll(rt_val,shamt);
            break;
        }
        case 0x02: { //srl: logical shift right
            bitwise bw;
            result = bw.bitwise_srl(rt_val,shamt);
            break;
        }
        case 0x03: { //sra: arithmetic shift right
            bitwise bw;
            result = bw.bitwise_sra(rt_val,shamt);
            break;
        }
        case 0x04: { //sllv: logical shift left variable
            if (shamt != 0) {
                std::cerr << "Invalid or malformed instruction" << std::endl;
                std::exit(-12);
            }
            bitwise bw;
            result = bw.bitwise_sll(rt_val,rs_val);
            break;
        }
        case 0x06: { //srlv: logical shift right variable
            if (shamt != 0) {
                std::cerr << "Invalid or malformed instruction" << std::endl;
                std::exit(-12);
            }
            bitwise bw;
            result = bw.bitwise_srl(rt_val,rs_val);
            break;
        }
        case 0x07: { //srav: arithmetic shift right variable
            if (shamt != 0) {
                std::cerr << "Invalid or malformed instruction" << std::endl;
                std::exit(-12);
            }
            bitwise bw;
            result = bw.bitwise_sra(rt_val,rs_val);
            break;
        }
        case 0x08: { //jr: jump to address in register
            // This is redundant, should be caught by simulator
            return rs_val;
            break;
        }
        case 0x09: { //jalr: jump and link register
            // This is redundant, should be caught by simulator
            return rs_val;
            break;
        }
        case 0x10: { //mfhi: move from hi register
            return hi_register;
            break;
        }
        case 0x11: { //mthi: move to hi register}
            hi_register = rs_val;
            return hi_register;
            break;
        }
        case 0x12: { //mflo: move from lo register
            return lo_register;
            break;
        }
        case 0x13: { //mtlo: move to lo register
            lo_register = rs_val;
            return lo_register;
            break;
        }
        case 0x18: { //mult: multiply
            arithmetic alu;
            // both positive(same as unsigned)
            if (rs_val>>31 == 0 && rt_val>>31 == 0) {
                long long multiply_result = alu.multiply_unsigned(rs_val, rt_val);
                long long multiply_hi = multiply_result>>32;
                long long multiply_lo = multiply_result&0xFFFFFFFF;
                hi_register = multiply_hi&0xFFFFFFFF;
                lo_register = multiply_lo&0xFFFFFFFF;
            }
            // rs positive, rt negative (invert rt and result)
            else if (rs_val>>31 == 0 && rt_val>>31 == 1) {
                rt_val = alu.invert(rt_val);
                long long multiply_result = alu.multiply_unsigned(rs_val, rt_val);
                if ((multiply_result&0xFFFFFFFF) == 0x00) {
                    lo_register = 0;
                    hi_register = alu.invert(multiply_result>>32);
                }
                else {
                    lo_register = alu.invert(multiply_result&0xFFFFFFFF);
                    hi_register = ~(multiply_result>>32);
                }
            }
            // rs negative, rt positive (invert rs and result)
            else if (rs_val>>31 == 1 && rt_val>>31 == 0) {
                rs_val = alu.invert(rs_val);
                long long multiply_result = alu.multiply_unsigned(rs_val, rt_val);
                if ((multiply_result&0xFFFFFFFF) == 0x00) {
                    lo_register = 0;
                    hi_register = alu.invert(multiply_result>>32);
                }
                else {
                    lo_register = alu.invert(multiply_result&0xFFFFFFFF);
                    hi_register = ~(multiply_result>>32);
                }
            }
            // both negative(invert both)
            else {
                rs_val = alu.invert(rs_val);
                rt_val = alu.invert(rt_val);
                long long multiply_result = alu.multiply_unsigned(rs_val, rt_val);
                long long multiply_hi = multiply_result>>32;
                long long multiply_lo = multiply_result&0xFFFFFFFF;
                hi_register = (operand)multiply_hi;
                lo_register = (operand)multiply_lo;
            }
            break;
        }
        case 0x19: { //multu: unsigned multiply
            arithmetic alu;
            long long multiply_result = alu.multiply_unsigned(rs_val, rt_val);
            long long multiply_hi = multiply_result>>32;
            long long multiply_lo = multiply_result&0xFFFFFFFF;
            hi_register = (operand)multiply_hi;
            lo_register = (operand)multiply_lo;
            break;
        }
        case 0x1A: { //div: division
            if (rt_val == 0) {
                std::cerr << "Arithmetic Exception: Divide by Zero" << std::endl;
                std::exit(-10);
            }
            arithmetic alu;
            // both positive(same as unsigned)
            if (rs_val>>31 == 0 && rt_val>>31 == 0) {
                lo_register = alu.division_quotient(rs_val, rt_val);
                hi_register = alu.division_remainder(rs_val, rt_val);
            }
            // rs positive, rt negative(negative quotient, positive remainder)
            else if (rs_val>>31 == 0 && rt_val>>31 == 1) {
                rt_val = alu.invert(rt_val);
                operand quotient = alu.division_quotient(rs_val, rt_val);
                operand remainder = alu.division_remainder(rs_val, rt_val);
                lo_register = alu.invert(quotient);
                hi_register = remainder;
            }
            // rs negative, rt positive(negative quotient, negative remainder)
            else if (rs_val>>31 == 1 && rt_val>>31 == 0) {
                rs_val = alu.invert(rs_val);
                operand quotient = alu.division_quotient(rs_val, rt_val);
                operand remainder = alu.division_remainder(rs_val, rt_val);
                lo_register = alu.invert(quotient);
                hi_register = alu.invert(remainder);
            }
            // both negative(positive quotient, negative remainder)
            else {
                rs_val = alu.invert(rs_val);
                rt_val = alu.invert(rt_val);
                lo_register = alu.division_quotient(rs_val, rt_val);
                operand remainder = alu.division_remainder(rs_val, rt_val);
                hi_register = alu.invert(remainder);
            }
            break;
        }
        case 0x1B: { //divu: unsigned division
            if (rt_val == 0) {
                std::cerr << "Arithmetic Exception: Divide by Zero" << std::endl;
                std::exit(-10);
            }
            arithmetic alu;
            lo_register = alu.division_quotient(rs_val, rt_val);
            hi_register = alu.division_remainder(rs_val, rt_val);
            break;
        }
        case 0x20: { //add: addition
            if (shamt != 0) {
                std::cerr << "Invalid or malformed instruction" << std::endl;
                std::exit(-12);
            }
            arithmetic alu;
            result = alu.add_unsigned(rs_val, rt_val);
            // If both positive and result is negative
            if (rs_val>>31 == 0 && rt_val>>31 == 0 && result >> 31 == 1) {
                std::cerr << "Arithmetic Exception: Overflow" << std::endl;
                std::exit(-10);
            }
            // If both negative and result is positive
            else if (rs_val>>31 == 1 && rt_val>>31 == 1 && result >> 31 == 0) {
                std::cerr << "Arithmetic Exception: Overflow" << std::endl;
                std::exit(-10);
            }
            break;
        }
        case 0x21: { //addu: unsigned addition
            if (shamt != 0 ) {
                std::cerr << "Invalid or malformed instruction" << std::endl;
                std::exit(-12);
            }
            arithmetic alu;
            result = alu.add_unsigned(rs_val,rt_val);
            break;
        }
        case 0x22: { //sub: subtraction
            if (shamt != 0) {
                std::cerr << "Invalid or malformed instruction" << std::endl;
                std::exit(-12);
            }
            arithmetic alu;
            result = alu.sub_unsigned(rs_val,rt_val);
            // If rs is positive and rt is negative and result is negative
            if (rs_val>>31 == 0 && rt_val>>31 == 1 && result >> 31 == 1) {
                std::cerr << "Arithmetic Exception: Overflow" << std::endl;
                std::exit(-10);
            }
            // If rs is negative and rt is positive and result is positive
            else if (rs_val>>31 == 1 && rt_val>>31 == 0 && result >> 31 == 0) {
                std::cerr << "Arithmetic Exception: Overflow" << std::endl;
                std::exit(-10);
            }
            break;
        }
        case 0x23: { //subu: unsigned subtraction
            if (shamt != 0) {
                std::cerr << "Invalid or malformed instruction" << std::endl;
                std::exit(-12);
            }
            arithmetic alu;
            result = alu.sub_unsigned(rs_val,rt_val);
            break;
        }
        case 0x24: { //and: bitwise and
            if (shamt != 0) {
                std::cerr << "Invalid or malformed instruction" << std::endl;
                std::exit(-12);
            }
            bitwise bw;
            result = bw.bitwise_and(rs_val,rt_val);
            break;
        }
        case 0x25: { //or: bitwise or
            if (shamt != 0) {
                std::cerr << "Invalid or malformed instruction" << std::endl;
                std::exit(-12);
            }
            bitwise bw;
            result = bw.bitwise_or(rs_val,rt_val);
            break;
        }
        case 0x26: { //xor: bitwise xor
            if (shamt != 0) {
                std::cerr << "Invalid or malformed instruction" << std::endl;
                std::exit(-12);
            }
            bitwise bw;
            result = bw.bitwise_xor(rs_val,rt_val);
            break;
        }
        case 0x27: { //nor: bitwise nor
            if (shamt != 0) {
                std::cerr << "Invalid or malformed instruction" << std::endl;
                std::exit(-12);
            }
            bitwise bw;
            result = bw.bitwise_nor(rs_val,rt_val);
            break;
        }
        case 0x2A: { //slt: set to 1 if less than
            if (shamt != 0) {
                std::cerr << "Invalid or malformed instruction" << std::endl;
                std::exit(-12);
            }
            if (rt_val>>31 == 1) { // If rt is negative
                if (rs_val>>31 == 0) { // If rt is negative and rs is positive
                    result = 0;
                }
                else { // If both negative
                    arithmetic alu;
                    result = alu.set_less_than_unsigned(rs_val, rt_val);
                }
            }
            else if (rs_val>>31 == 1){ // If rt is positive and rs is negative
                result = 1;
            }
            else { //Both positive
                arithmetic alu;
                result = alu.set_less_than_unsigned(rs_val,rt_val);
            }
            break;
        }
        case 0x2B: { //sltu: set to 1 if less than unsigned
            if (shamt != 0) {
                std::cerr << "Invalid or malformed instruction" << std::endl;
                std::exit(-12);
            }
            arithmetic alu;
            result = alu.set_less_than_unsigned(rs_val,rt_val);
            break;
        }
        default: {
            std::cerr << "Invalid or unsupported instruction" << std::endl;
            std::exit(-20);
        }
    }
    return result;
}


/*
 * This function sign extends the 8-bit value n to 32 bits and returns it
 */
uint32_t processor::sign_extend8(uint32_t data) {
    uint32_t sign = (data>>7)&0x1;
	if(sign == 0) {
		//leading 0's by default
		return data;
	} else {
		// leading bit == 1
		// bits 31:16 should all be 1 without affecting first 16 bits
		return (data|0xFFFFFF00);
	}
}

/*
 * This function sign extends the 16-bit n word to 32 bits and returns it
 */
uint32_t processor::sign_extend16(uint32_t data) {
	uint32_t sign = (data>>15)&0x1;
	if(sign == 0) {
		//leading 0's by default
		return data;
	} else {
		// leading bit == 1
		// bits 31:16 should all be 1 without affecting first 16 bits
		return (data|0xFFFF0000);
	}
}

uint32_t processor::sign_extend18(uint32_t data) {
	uint32_t sign = (data>>17)&0x1;
	if(sign == 0) {
		//leading 0's by default
		return data;
	} else {
		// leading bit == 1
		// bits 31:19 should all be 1 without affecting first 16 bits
		return (data|0xFFFC0000);
	}
}

bool processor::greater_than_zero(operand data) {
    return ((data>>31)==0 && data!=0);
}

bool processor::equal_zero(operand data) {
    return (data==0);
}

bool processor::less_than_zero(operand data) {
    return ((data>>31)==1);
}
