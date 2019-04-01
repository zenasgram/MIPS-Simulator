#include "simulator.hpp"

instruction simulator::fetch_instruction() {
    mem_address current_address = pc.get_current_address();
    if (current_address == 0) {
        std::exit(rf.read_register(2)&0xFF);
    }
    else {
        instruction current_instruction = im.fetch_instruction(current_address);
        pc.increment();
        return current_instruction;
    }
}

int simulator::decode_and_execute_instruction(instruction inst) {
    operand opcode = inst>>26;
    switch (opcode) {
        case 0x00: {// R Type Instructions
            operand funct = inst&0x3F;
            operand rs = (inst>>21)&0x1F;
            operand rt = (inst>>16)&0x1F;
            operand rd = (inst>>11)&0x1F;
            operand shamt = (inst>>6)&0x1F;
            operand rs_val = rf.read_register(rs);
            operand rt_val = rf.read_register(rt);
            if (funct == 0x00 || funct == 0x02 || funct == 0x03) {
                if (rs != 0) {
                    std::cerr << "Invalid or malformed instruction" << std::endl;
                    std::exit(-12);
                }
            }
            if (funct == 0x08) {
                if (rt != 0 || rd != 0) {
                    std::cerr << "Invalid or malformed instruction" << std::endl;
                    std::exit(-12);
                }
                execute_delayed_slot();
                pc.set_address(rs_val);
            }
            else if (funct == 0x09) {
                if (rt != 0) {
                    std::cerr << "Invalid or malformed instruction" << std::endl;
                    std::exit(-12);
                }
                execute_delayed_slot();
                rf.write_register(rd,pc.get_current_address());
                pc.set_address(rs_val);
            }
//            else if (funct == 0x10 || funct == 0x12) {
//                if (rs != 0 || rt != 0 || shamt != 0) {
//                    std::cerr << "Invalid or malformed instruction" << std::endl;
//                    std::exit(-12);
//                }
//            }
//            else if (funct == 0x11 || funct == 0x13) {
//                if (rt != 0 || rd != 0 || shamt != 0) {
//                    std::cerr << "Invalid or malformed instruction" << std::endl;
//                    std::exit(-12);
//                }
//            }
//            else if (funct == 0x18 || funct == 0x19 || funct == 0x1A || funct == 0x1B) {
//                if (rd != 0 || shamt != 0) {
//                    std::cerr << "Invalid or malformed instruction" << std::endl;
//                    std::exit(-12);
//                }
//            }
            else {
                operand result = ip.execute_r_instruction(funct, rs_val, rt_val, shamt);
                rf.write_register(rd, result);
            }
            break;
        }
        case 0x01: {// bltz
            operand rs = (inst>>21)&0x1F;
            operand rs_val = rf.read_register(rs);
            operand funct = (inst>>16)&0x1F;
            operand offset = (inst&0xFFFF)<<2;
            switch (funct) {
                case 0x00: {//bltz
                    if (ip.less_than_zero(rs_val)) {
                        execute_delayed_slot();
                        pc.jump_offset(ip.sign_extend18(offset)-4);
                    }
                    break;
                }
                case 0x01: {//bgez
                    if (ip.greater_than_zero(rs_val)||ip.equal_zero(rs_val)) {
                        execute_delayed_slot();
                        pc.jump_offset(ip.sign_extend18(offset)-4);
                    }
                    break;
                }
                case 0x10: {//bltzal
                    if (ip.less_than_zero(rs_val)) {
                        execute_delayed_slot();
                        rf.write_register(31,pc.get_current_address());
                        pc.jump_offset(ip.sign_extend18(offset)-4);
                    }
                    break;
                }
                case 0x11: {//bgezal
                    if (ip.greater_than_zero(rs_val)||ip.equal_zero(rs_val)) {
                        execute_delayed_slot();
                        rf.write_register(31,pc.get_current_address());
                        pc.jump_offset(ip.sign_extend18(offset)-4);
                    }
                    break;
                }
                default: {
                    std::cerr << "Invalid or unsupported instruction" << std::endl;
                    std::exit(-20);
                }
            }
            break;
        }
        case 0x02: {// Jump
            execute_delayed_slot();
            pc.set_address(((inst&0x3FFFFFF)<<2)+0x10000000);
            break;
        }
        case 0x03: {// Jump and Link
            execute_delayed_slot();
            rf.write_register(31,pc.get_current_address());
            pc.set_address(((inst&0x3FFFFFF)<<2)+0x10000000);
            break;
        }
        case 0x04: {//beq
            operand rs = (inst>>21)&0x1F;
            operand rt = (inst>>16)&0x1F;
            operand rs_val = rf.read_register(rs);
            operand rt_val = rf.read_register(rt);
            if (rs_val == rt_val) {
                operand offset = (inst&0xFFFF)<<2;
                execute_delayed_slot();
                pc.jump_offset(ip.sign_extend18(offset)-4);
            }
            break;
        }
        case 0x05: {//bne
            operand rs = (inst>>21)&0x1F;
            operand rt = (inst>>16)&0x1F;
            operand rs_val = rf.read_register(rs);
            operand rt_val = rf.read_register(rt);
            operand result = ip.execute_r_instruction(0x23, rs_val, rt_val, 0);
            if (result != 0) {
                execute_delayed_slot();
                operand offset = (inst&0xFFFF)<<2;
                pc.jump_offset(ip.sign_extend18(offset)-4);
            }
            break;
        }
        case 0x06: {//blez
            if (((inst>>16)&0x1F)!=0) {
                std::cerr << "Invalid or unsupported instruction" << std::endl;
                std::exit(-20);
            }
            operand rs = (inst>>21)&0x1F;
            operand rs_val = rf.read_register(rs);
            if (ip.less_than_zero(rs_val)||ip.equal_zero(rs_val)) {
                execute_delayed_slot();
                operand offset = (inst&0xFFFF)<<2;
                pc.jump_offset(ip.sign_extend18(offset)-4);
            }
            break;
        }
        case 0x07: {//bgtz
            if (((inst>>16)&0x1F)!=0) {
                std::cerr << "Invalid or unsupported instruction" << std::endl;
                std::exit(-20);
            }
            operand rs = (inst>>21)&0x1F;
            operand rs_val = rf.read_register(rs);
            if (ip.greater_than_zero(rs_val)) {
                execute_delayed_slot();
                operand offset = (inst&0xFFFF)<<2;
                pc.jump_offset(ip.sign_extend18(offset)-4);
            }
            break;
        }
        case 0x08: {//addi
            operand rs = (inst>>21)&0x1F;
            operand rt = (inst>>16)&0x1F;
            operand rs_val = rf.read_register(rs);
            operand imm = inst&0xFFFF;
            imm = ip.sign_extend16(imm);
            operand result = ip.execute_r_instruction(0x20, rs_val, imm, 0);
            if ((imm>>31) == 0 && (rs_val>>31) == 0 && (result>>31) == 1) {
                std::cerr << "Arithmetic Exception: Overflow" << std::endl;
                std::exit(-10);
            }
            else if ((imm>>31) == 1 && (rs_val>>31) == 1 && (result>>31) == 0) {
                std::cerr << "Arithmetic Exception: Overflow" << std::endl;
                std::exit(-10);
            }
            rf.write_register(rt, result);
            break;
        }
        case 0x09: {//addiu
            operand rs = (inst>>21)&0x1F;
            operand rt = (inst>>16)&0x1F;
            operand rs_val = rf.read_register(rs);
            operand imm = inst&0xFFFF;
            imm = ip.sign_extend16(imm);
            operand result = ip.execute_r_instruction(0x21, rs_val, imm, 0);
            rf.write_register(rt, result);
            break;
        }
        case 0x0A: {//slti
            operand rs = (inst>>21)&0x1F;
            operand rt = (inst>>16)&0x1F;
            operand rs_val = rf.read_register(rs);
            operand imm = inst&0xFFFF;
            imm = ip.sign_extend16(imm);
            operand result = ip.execute_r_instruction(0x2A, rs_val, imm, 0);
            rf.write_register(rt, result);
            break;
        }
        case 0x0B: {//sltiu
            operand rs = (inst>>21)&0x1F;
            operand rt = (inst>>16)&0x1F;
            operand rs_val = rf.read_register(rs);
            operand imm = inst&0xFFFF;
            imm = ip.sign_extend16(imm);
            operand result = ip.execute_r_instruction(0x2B, rs_val, imm, 0);
            rf.write_register(rt, result);
            break;
        }
        case 0x0C: {//andi
            operand rs = (inst>>21)&0x1F;
            operand rt = (inst>>16)&0x1F;
            operand rs_val = rf.read_register(rs);
            operand imm = inst&0xFFFF;
            operand result = ip.execute_r_instruction(0x24, rs_val, imm, 0);
            rf.write_register(rt, result);
            break;
        }
        case 0x0D: {//ori
            operand rs = (inst>>21)&0x1F;
            operand rt = (inst>>16)&0x1F;
            operand rs_val = rf.read_register(rs);
            operand imm = inst&0xFFFF;
            operand result = ip.execute_r_instruction(0x25, rs_val, imm, 0);
            rf.write_register(rt, result);
            break;
        }
        case 0x0E: {//xori
            operand rs = (inst>>21)&0x1F;
            operand rt = (inst>>16)&0x1F;
            operand rs_val = rf.read_register(rs);
            operand imm = inst&0xFFFF;
            operand result = ip.execute_r_instruction(0x26, rs_val, imm, 0);
            rf.write_register(rt, result);
            break;
        }
        case 0x0F: {//lui
            operand rt = (inst>>16)&0x1F;
            operand imm = inst&0xFFFF;
            imm = imm<<16;
            rf.write_register(rt, imm);
            break;
        }
        case 0x23: {//lw
            operand rs = (inst>>21)&0x1F;
            operand rt = (inst>>16)&0x1F;
            operand rs_val = rf.read_register(rs);
            operand imm = inst&0xFFFF;
            imm = ip.sign_extend16(imm);
            operand data_word;
            int result = im.load_word(rs_val+imm, data_word);
            if (result == 0) {
                rf.write_register(rt, data_word);
            }
            else if (result == 1) {
                pc.jump_offset(-4);
                return 1;
            }
            else if (result == 2) {
                data_word = ip.sign_extend8(data_word);
                rf.write_register(rt, data_word);
            }
            break;
        }
        case 0x20: {//lb
            operand rs = (inst>>21)&0x1F;
            operand rt = (inst>>16)&0x1F;
            operand rs_val = rf.read_register(rs);
            operand imm = inst&0xFFFF;
            imm = ip.sign_extend16(imm);
            char data_byte;
            int result = im.load_byte(rs_val+imm, data_byte);
            if (result == 0) {
                operand data_word = ip.sign_extend8(data_byte);
                rf.write_register(rt, data_word);
            }
            else if (result == 1) {
                pc.jump_offset(-4);
                return 1;
            }
            else if (result == 2) {
                operand data_word = ip.sign_extend8(data_byte);
                rf.write_register(rt, data_word);
            }
            break;
        }
        case 0x21: {//lh
            operand rs = (inst>>21)&0x1F;
            operand rt = (inst>>16)&0x1F;
            operand rs_val = rf.read_register(rs);
            operand imm = inst&0xFFFF;
            operand halfword=0;
            imm = ip.sign_extend16(imm);
            char data_byte;
            if (((rs_val+imm)%2)!=0) {
                std::cerr << "Memory Exception: Non Aligned Halfword" << std::endl;
                std::exit(-11);
            }
            int result = im.load_byte(rs_val+imm, data_byte);
            if (result == 0) {
                halfword = halfword|(data_byte&0xFF);
                im.load_byte(rs_val+imm+1, data_byte);
                halfword = (halfword<<8)|(data_byte&0xFF);
                rf.write_register(rt, ip.sign_extend16(halfword));
            }
            else if (result == 1) {
                pc.jump_offset(-4);
                return 1;
            }
            else if (result == 2) {
                rf.write_register(rt, ip.sign_extend8(data_byte));
            }
            break;
        }
        case 0x22: {//lwl: Load word left
            operand rs = (inst>>21)&0x1F;
            operand rt = (inst>>16)&0x1F;
            operand rs_val = rf.read_register(rs);
            operand imm = inst&0xFFFF;
            imm = ip.sign_extend16(imm);
            operand load_address = rs_val+imm;
            operand data_word = 0;
            switch (load_address%4) {
                case 0: {
                    int result = im.load_word(load_address,data_word);
                    if (result == 1) {
                        pc.jump_offset(-4);
                        return 1;
                    }
                    else if (result == 2) {
                        data_word = ip.sign_extend8(data_word);
                    }
                    break;
                }
                case 1: {
                    char data_byte;
                    im.load_byte(load_address, data_byte);
                    data_word = (data_word|data_byte)<<8;
                    im.load_byte(load_address+1, data_byte);
                    data_word = (data_word|data_byte)<<8;
                    im.load_byte(load_address+2, data_byte);
                    data_word = (data_word|data_byte)<<8;
                    break;
                }
                case 2: {
                    char data_byte;
                    im.load_byte(load_address, data_byte);
                    data_word = (data_word|data_byte)<<8;
                    im.load_byte(load_address+1, data_byte);
                    data_word = (data_word|data_byte)<<16;
                    break;
                }
                case 3: {
                    char data_byte;
                    im.load_byte(load_address, data_byte);
                    data_word = (data_word|data_byte)<<24;
                    break;
                }
            }
            rf.write_register(rt,data_word);
            break;
        }
        case 0x24: {//lbu
            operand rs = (inst>>21)&0x1F;
            operand rt = (inst>>16)&0x1F;
            operand rs_val = rf.read_register(rs);
            operand imm = inst&0xFFFF;
            imm = ip.sign_extend16(imm);
            char data_byte;
            int result = im.load_byte(rs_val+imm, data_byte);
            if (result == 1) {
                pc.jump_offset(-4);
                return 1;
            }
            operand data_word = data_byte&0xFF;
            rf.write_register(rt, data_word);
            break;
        }
        case 0x25: {//lhu
            operand rs = (inst>>21)&0x1F;
            operand rt = (inst>>16)&0x1F;
            operand rs_val = rf.read_register(rs);
            operand imm = inst&0xFFFF;
            operand halfword = 0;
            imm = ip.sign_extend16(imm);
            char data_byte;
            if (((rs_val+imm)%2)!=0) {
                std::cerr << "Memory Exception: Non Aligned Halfword" << std::endl;
                std::exit(-11);
            }
            int result = im.load_byte(rs_val+imm, data_byte);
            if (result == 0) {
                halfword = halfword|(data_byte&0xFF);
                im.load_byte(rs_val+imm+1, data_byte);
                halfword = (halfword<<8)|(data_byte&0xFF);
            }
            else if (result == 1) {
                return 1;
            }
            else if (result == 2) {
                halfword = data_byte;
            }
            rf.write_register(rt, halfword);
            break;
        }
        case 0x26: {//lwr
            operand rs = (inst>>21)&0x1F;
            operand rt = (inst>>16)&0x1F;
            operand rs_val = rf.read_register(rs);
            operand imm = inst&0xFFFF;
            imm = ip.sign_extend16(imm);
            operand load_address = rs_val+imm;
            operand data_word = 0;
            switch (load_address%4) {
                case 0: {
                    char data_byte;
                    im.load_byte(load_address, data_byte);
                    data_word = (data_word|data_byte);
                    break;
                }
                case 1: {
                    char data_byte;
                    im.load_byte(load_address-1, data_byte);
                    data_word = (data_word|data_byte)<<8;
                    im.load_byte(load_address, data_byte);
                    data_word = (data_word|data_byte);
                    break;
                }
                case 2: {
                    char data_byte;
                    im.load_byte(load_address-2, data_byte);
                    data_word = (data_word|data_byte)<<8;
                    im.load_byte(load_address-1, data_byte);
                    data_word = (data_word|data_byte)<<8;
                    im.load_byte(load_address, data_byte);
                    data_word = (data_word|data_byte);
                    break;
                }
                case 3: {
                    int result = im.load_word(load_address-3,data_word);
                    if (result == 1) {
                        pc.jump_offset(-4);
                        return 1;
                    }
                    else if (result == 2) {
                        data_word = ip.sign_extend8(data_word);
                    }
                    break;
                }
            }
            rf.write_register(rt,data_word);
            break;
        }
        case 0x28: {//sb
            operand rs = (inst>>21)&0x1F;
            operand rt = (inst>>16)&0x1F;
            operand rs_val = rf.read_register(rs);
            operand rt_val = rf.read_register(rt);
            operand imm = inst&0xFFFF;
            imm = ip.sign_extend16(imm);
            im.store_byte(rs_val+imm, rt_val&0xFF);
            break;
        }
        case 0x29: {//sh
            operand rs = (inst>>21)&0x1F;
            operand rt = (inst>>16)&0x1F;
            operand rs_val = rf.read_register(rs);
            operand rt_val = rf.read_register(rt);
            operand imm = inst&0xFFFF;
            imm = ip.sign_extend16(imm);
            if (((rs_val+imm)%2)!=0) {
                std::cerr << "Memory Exception: Non Aligned Halfword" << std::endl;
                std::exit(-11);
            }
            im.store_half(rs_val+imm, rt_val&0xFFFF);
            break;
        }
        case 0x2B: {//sw
            operand rs = (inst>>21)&0x1F;
            operand rt = (inst>>16)&0x1F;
            operand rs_val = rf.read_register(rs);
            operand rt_val = rf.read_register(rt);
            operand imm = inst&0xFFFF;
            imm = ip.sign_extend16(imm);
            im.store_word(rs_val+imm, rt_val);
            break;
        }
        default: {
            std::cerr << "Invalid or unsupported instruction" << std::endl;
            std::exit(-20);
        }
    }
    return 0;
}


simulator::simulator(std::vector<char> binary) {
    im.init(binary);
}

void simulator::start(mem_address memory_address) {
    pc.set_address(memory_address);
}

int simulator::step() {
    instruction current_instruction = fetch_instruction();
    int status = decode_and_execute_instruction(current_instruction);
    if (status == 1) {
        return 1;
    }
    return 0;
}

void simulator::execute_delayed_slot() {
    instruction current_instruction = fetch_instruction();
    decode_and_execute_instruction(current_instruction);
}

void simulator::store_input(char input) {
    im.store_byte(0x30000000,input);
}
