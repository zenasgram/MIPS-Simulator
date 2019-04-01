#ifndef REGISTER_FILE_HPP
#define REGISTER_FILE_HPP

#include <cstdint>

typedef uint32_t mips_register;
class register_file {
    private:
        const mips_register zero = 0; // Wired to 0
        mips_register at; // Reserved for pseudo instructions
        mips_register v0, v1; // Return values from functions
        mips_register a0, a1, a2, a3; // Arguments to functions, not preserved by subprograms
        mips_register t0, t1, t2, t3, t4, t5, t6, t7; // Temporary Registers, not preserved by subprograms
        mips_register s0, s1, s2, s3, s4, s5, s6, s7; // Saved registers, preserved by subprograms
        mips_register t8, t9; // Temporary Registers, not preserved by subprograms
        mips_register k0, k1; //Reserved for kernel
        mips_register gp; // Global Area Pointer
        mips_register sp; // Stack Pointer
        mips_register fp; // Frame Pointer
        mips_register ra; // Return Address
    public:
        register_file();
        mips_register read_register(int register_number);
        void write_register(int register_number, mips_register input);
};
#endif
