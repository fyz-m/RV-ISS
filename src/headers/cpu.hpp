#pragma  once
#include "memory.hpp"
#include <cstdint>
#include <memory>


enum class OPERATION {

  UNKNOWN,
  // R-type
  ADD, SUB, SLL, SLT, SLTU, XOR, SRL, SRA, OR, AND, 
  MUL, MULH, MULHSU, MULHU, DIV, DIVU, REM, REMU, 
  // I-type
  LB, LH, LW, LBU, LHU, ADDI, SLLI, SLTI, SLTIU, XORI, SRLI, SRAI, ORI, ANDI, JALR,
  // S-type
  SB, SH, SW,
  // B-type
  BEQ, BNE, BLT, BGE, BLTU, BGEU,
  // U-type
  LUI, AUIPC,
  // J-type
  JAL,

  EBREAK = 0x00100073 
};

enum class TYPE { UNKNOWN, R_TYPE, I_TYPE, S_TYPE, B_TYPE, U_TYPE, J_TYPE };

typedef struct  DecodedInstruction {

  uint32_t raw_inst {};
  uint8_t opcode{}, funct3{}, funct7{}, rs1{}, rs2{}, rd{};
  int32_t imm{};
  TYPE type = TYPE::UNKNOWN;
  OPERATION Operation = OPERATION::UNKNOWN;

} DecodedInstruction;


class CPU
{
  private:

    // Contains currently executing instruction
    uint32_t instruction_register;

    // Contains instruction fields (opcode, rs1, rd etc.)
    DecodedInstruction instruction_fields;

    // Contains memory address of instruction to execute
    uint32_t program_counter;

    // Stores operands to perform operations on 
    RegisterFile register_file;

    // Memory containing instructions 
    std::shared_ptr <Memory> Instruction_Memory;

    // Memory for data
    std::shared_ptr <Memory> Data_Memory;

    int cycle_count {};

  public:

    CPU(int width = 32, 
        std::shared_ptr<Memory> Instruction_Memory_ptr = nullptr, 
        std::shared_ptr<Memory> Data_Memory_ptr = nullptr);
    
    friend class CPU_test;

    void runtoEbreak();

    // Complete one instruction (Fetch/Decode/Execute)
    void Step();

    // Returns the pointer of the memory object CPU is fetching instructions from
    std::shared_ptr<Memory> getInstructionMemory() const; 

    // Returns the pointer of the memory object CPU is using for data reads/writes
    std::shared_ptr<Memory> getDataMemory() const; 

    // Returns number of cycles (1 cycle = 1 F/D/E loop)
    int cycles();
 
    // Fetch instruction at address[PC] and write it to the instruction register
    void Fetch();

    // Decode the instruction, determining the operation to perform and the operands
    void Decode();

    // Execute the operation
    void Execute();
    
    // Returns the current value of the program counter
    uint32_t readPC() const;

    // Write a value to the program counter
    void writePC(const uint32_t &data);
    
    // Increment program counter to the next instruction
    void incrementPC();
    
    // Returns the value in the register[address]
    uint32_t readReg(const int &address) const;
    
    // Prints the architectural state of the CPU (registers + pc)
    void printState();
    
  
  private:

    // Write to register[address]
    void writeReg(const int &address, const uint32_t &data);

    void execute_R_type();

    void execute_I_type();

    void execute_S_type();

    void execute_B_type();

    void execute_U_type();

    void execute_J_type();
};



// Expose instruction fields for testing
class CPU_test : public CPU {

  public:
    using CPU::CPU;
    using CPU::instruction_fields;
    using CPU::writeReg;
};

