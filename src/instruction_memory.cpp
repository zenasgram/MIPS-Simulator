#include "instruction_memory.hpp"


#define ADDR_NULL_OFFSET 0x00000000
#define ADDR_INSTR_OFFSET 0x10000000
#define ADDR_DATA_OFFSET 0x20000000
#define ADDR_GETC_OFFSET 0x30000000
#define ADDR_PUTC_OFFSET 0x30000004

#define TRANS_NULL_OFFSET 0x0000000
#define TRANS_INSTR_OFFSET 0x4
#define TRANS_DATA_OFFSET 0x1000004
#define TRANS_GETC_OFFSET 0x5000004
#define TRANS_PUTC_OFFSET 0x5000008

#define ADDR_NULL_LENGTH 0x4
#define ADDR_INSTR_LENGTH 0x1000000
#define ADDR_DATA_LENGTH 0x4000000
#define ADDR_GETC_LENGTH 0x4
#define ADDR_PUTC_LENGTH 0x4


#define ADDR_EMPTY1_OFFSET 0x00000004
#define ADDR_EMPTY2_OFFSET 0x11000000
#define ADDR_EMPTY3_OFFSET 0x24000000
#define ADDR_EMPTY4_OFFSET 0x30000008

#define ADDR_EMPTY1_LENGTH 0xFFFFFFC
#define ADDR_EMPTY2_LENGTH 0xF000000
#define ADDR_EMPTY3_LENGTH 0xC000000
#define ADDR_EMPTY4_LENGTH 0xCFFFFFF8


#define FULLRANGE_LENGTH 0x500000C // translated fullrange





//default constructor
instruction_memory::instruction_memory(){
   set_offset(0x0);
   set_length(FULLRANGE_LENGTH);
   getc_wait = false;
}

instruction_memory::instruction_memory(uint32_t offset_in, uint32_t length_in): offset(offset_in), length(length_in){}



//GET AND SET OFFSET/LENGTH
uint32_t instruction_memory::get_offset() const{ return offset;}
uint32_t instruction_memory::get_length() const{ return length;}
void instruction_memory::set_offset (uint32_t offset_in){ offset = offset_in;}
void instruction_memory::set_length (uint32_t length_in){ length = length_in;}


void instruction_memory::init(std::vector<type>& binaries){
    insmem = std::vector<type>(length,0);
    load_instr(binaries); //LOAD instruction memory
};

void instruction_memory::load_instr(std::vector<type>& binaries){
        for(uint32_t i=0; i<binaries.size(); i++){
          insmem[i+TRANS_INSTR_OFFSET] = binaries[i];
      }
}

//========================================================================================
//INSTRUCTION MEMORY LOGIC
//************************

uint32_t instruction_memory::fetch_instruction(uint32_t PC){
    if (PC == ADDR_NULL_OFFSET) {
        // Should not happen as simulator should exit if PC is 0
        std::exit(0);
    }
    else if (PC >= ADDR_INSTR_OFFSET && PC <= (ADDR_INSTR_OFFSET+ADDR_INSTR_LENGTH) ){
        uint32_t instr_address = (PC - ADDR_INSTR_OFFSET) + TRANS_INSTR_OFFSET;
        return get_word(instr_address);
    }
    else if (PC <= ADDR_EMPTY4_OFFSET){
        std::cerr << "Memory Exception: Executing Invalid Memory Address" << std::endl;
        std::exit(-11);
    }
    else {
        std::cerr << "Invalid Instruction: Address out of range" << std::endl;
        std::exit(-12);
    }
}

//========================================================================================

//Functions for storing and retrieving words from vector
uint32_t instruction_memory::get_word(uint32_t address){
    //bitmask with 0xFF to ensure 8 bit length
    uint32_t byte1, byte2, byte3, byte4;
    byte1 = uint32_t(insmem[address]) & 0x000000FF;
    byte2 = uint32_t(insmem[address+0x1]) & 0x000000FF;
    byte3 = uint32_t(insmem[address+0x2]) & 0x000000FF;
    byte4 = uint32_t(insmem[address+0x3]) & 0x000000FF;
    //Combine to word
    uint32_t word = ( (byte1 << 24) | (byte2 << 16) | (byte3 << 8) | byte4 );
    return word;
}

void instruction_memory::put_word(uint32_t address, uint32_t data){
    char byte1 = ( data & 0xFF000000 ) >> 24;
    char byte2 = ( data & 0x00FF0000 ) >> 16;
    char byte3 = ( data & 0x0000FF00 ) >> 8;
    char byte4 =   data & 0x000000FF;

    insmem[address] = byte1;
    insmem[address+0x1] = byte2;
    insmem[address+0x2] = byte3;
    insmem[address+0x3] = byte4;
}

//load and store functions
int instruction_memory::load_word(uint32_t address, uint32_t& data) {
    if ((address%4) != 0) {
        std::cerr << "Memory Exception: Non Word Aligned Read" << std::endl;
        std::exit(-11);
    }
    if (address >= ADDR_INSTR_OFFSET && address <= (ADDR_INSTR_OFFSET+ADDR_INSTR_LENGTH)) {
        uint32_t instr_address = (address - ADDR_INSTR_OFFSET) + TRANS_INSTR_OFFSET;
        data = get_word(instr_address);
        return 0;
    }
    else if (address >= ADDR_DATA_OFFSET && address <= (ADDR_DATA_OFFSET+ADDR_DATA_LENGTH)) {
        uint32_t data_address = (address - ADDR_DATA_OFFSET) + TRANS_DATA_OFFSET;
        data = get_word(data_address);
        return 0;
    }
    else if (address == ADDR_GETC_OFFSET && !getc_wait) {
        getc_wait = true;
        return 1;
    }
    else if (address == ADDR_GETC_OFFSET && getc_wait) {
        getc_wait = false;
        data = insmem[TRANS_GETC_OFFSET] & 0xFF;
        return 2;
    }
    else if (address <= ADDR_EMPTY4_OFFSET) {
        std::cerr << "Memory Exception: Invalid Read Address" << std::endl;
        std::exit(-11);
    }
    else {
        std::cerr << "Memory Exception: Address Out Of Range" << std::endl;
        std::exit(-11);
    }
}

void instruction_memory::store_word(uint32_t address, uint32_t data){
    if ((address%4) != 0) {
        std::cerr << "Memory Exception: Non Word Aligned Write" << std::endl;
        std::exit(-11);
    }
    if (address >= ADDR_DATA_OFFSET && address <= (ADDR_DATA_OFFSET+ADDR_DATA_LENGTH)){
        uint32_t data_address = (address - ADDR_DATA_OFFSET) + TRANS_DATA_OFFSET;
        put_word(data_address, data);
    }
    else if (address == ADDR_PUTC_OFFSET) {
        uint32_t data_address = (address - ADDR_PUTC_OFFSET) + TRANS_PUTC_OFFSET;
        put_word(data_address, data);
        addr_putc();
    }
    else if (address <= ADDR_EMPTY4_OFFSET) {
        std::cerr << "Memory Exception: Invalid Write Address" << std::endl;
        std::exit(-11);
    }
    else {
        std::cerr << "Memory Exception: Address Out Of Range" << std::endl;
        std::exit(-11);
    }
}


// ADDRESS GETC
//R: Location of memory mapped input. Read-only.
char instruction_memory::addr_getc(){

    system("pause");//block until a character is available (eg. if a key needs to be pressed)

    std::cout << "\nuser input: " << std::endl;

    int d;
    char c;
    while ((d = getchar()) != '\n') {

        c = (char)d;
        insmem[TRANS_GETC_OFFSET] = c; //stores read char c into the RAM
        // Return the 8-bit extended to 32-bits as the result of the memory read
        return char32_t(c);
    }
    // End of File, return -1
    return EOF;

}


// ADDRESS PUTC
//W:Location of memory mapped output. Write-only.
void instruction_memory::addr_putc() {
    std::cout << insmem[TRANS_PUTC_OFFSET+3] << std::endl; //print in main
}


//========================================================================================

int instruction_memory::load_byte(uint32_t address, char& data){
    if (address >= ADDR_INSTR_OFFSET && address <= (ADDR_INSTR_OFFSET+ADDR_INSTR_LENGTH)) {
        uint32_t instr_address = (address - ADDR_INSTR_OFFSET) + TRANS_INSTR_OFFSET;
        data = insmem[instr_address] & 0x000000FF;
        return 0;
    }
    if (address >= ADDR_DATA_OFFSET && address <= (ADDR_DATA_OFFSET+ADDR_DATA_LENGTH)){
        uint32_t data_address = (address - ADDR_DATA_OFFSET) + TRANS_DATA_OFFSET;
        data = insmem[data_address] & 0x000000FF;
        return 0;
    }
    else if (address >= ADDR_GETC_OFFSET && address <= (ADDR_GETC_OFFSET+ADDR_GETC_LENGTH) && !getc_wait) {
        getc_wait = true;
        return 1;
    }
    else if (address >= ADDR_GETC_OFFSET && address <= (ADDR_GETC_OFFSET+ADDR_GETC_LENGTH) && getc_wait) {
        getc_wait = false;
        data = insmem[TRANS_GETC_OFFSET] & 0xFF;
        return 2;
    }
    else if (address <= ADDR_EMPTY4_OFFSET) {
        std::cerr << "Memory Exception: Invalid Read Address" << std::endl;
        std::exit(-11);
    }
    else {
        std::cerr << "Memory Exception: Address Out Of Range" << std::endl;
        std::exit(-11);
    }
}

void instruction_memory::store_byte(uint32_t address, char data){
    if (address >= ADDR_DATA_OFFSET && address <= (ADDR_DATA_OFFSET+ADDR_DATA_LENGTH)) {
        uint32_t data_address = (address - ADDR_DATA_OFFSET) + TRANS_DATA_OFFSET;
        insmem[data_address] = data;
    }
    else if (address >= ADDR_PUTC_OFFSET && address <= (ADDR_PUTC_OFFSET+ADDR_PUTC_LENGTH)) {
        uint32_t data_address = (address - ADDR_PUTC_OFFSET) + TRANS_PUTC_OFFSET;
        insmem[data_address] = data;
        addr_putc();
    }
    else if (address <= ADDR_EMPTY4_OFFSET) {
        std::cerr << "Memory Exception: Invalid Write Address" << std::endl;
        std::exit(-11);
    }
    else {
        std::cerr << "Memory Exception: Address Out Of Range" << std::endl;
        std::exit(-11);
    }
}

void instruction_memory::store_half(uint32_t address, uint32_t data){
    if (address >= ADDR_PUTC_OFFSET && address <= (ADDR_PUTC_OFFSET+ADDR_PUTC_LENGTH)) {
        uint32_t data_address = (address - ADDR_PUTC_OFFSET) + TRANS_PUTC_OFFSET;
        insmem[data_address] = (data>>8);
        insmem[data_address] = (data&0xFF);
        addr_putc();
    }
    else {
        store_byte(address, ((data>>8)&0xFF));
        store_byte(address+1, data&0xFF);
    }
}
