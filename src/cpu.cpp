#include "headers/cpu.hpp"
#include "headers/memory.hpp"
#include "headers/decode.hpp"
#include <cstdint>
#include <iostream>
#include <memory>

// TODO:
// Change instruction_fields.raw_inst to be a pointer to the instruction register
// To prevent unneccesary copying each time instruction is fetched

  CPU::CPU(int width, 
           std::shared_ptr<Memory> Instruction_Memory_ptr, 
           std::shared_ptr<Memory> Data_Memory_ptr) 
           
    : instruction_register{}, instruction_fields{}, program_counter{},
      register_file(width), 
      Instruction_Memory(Instruction_Memory_ptr), 
      Data_Memory(Data_Memory_ptr)
    {}

  void CPU::run_till_ebreak()
  {
    while(cycle_count < 100)
    {
     if (program_counter == static_cast<uint32_t>(OPERATION::EBREAK))
          return;

      Fetch();  
      Decode();
      Execute();
      std::cout << "cycle: " << cycle_count << std::endl;
      cycle_count++;
    }
  }

  void CPU::Step()
  {
    Fetch();
    Decode();
    Execute();
    cycle_count++;
  }

  void CPU::Fetch()
  {
    instruction_register = Instruction_Memory->Read_Word(program_counter);
    instruction_fields.raw_inst = instruction_register;
  }

  void CPU::Decode()
  {
    decode(instruction_fields);
  }

  void CPU::Execute()
  {
    switch (instruction_fields.type) 
    {
      case TYPE::R_TYPE: 
        execute_R_type();
        return;
        
      case TYPE::I_TYPE:
        execute_I_type();
        return;        

      case TYPE::S_TYPE:
        execute_S_type();
        return;
              
      case TYPE::B_TYPE:
        execute_B_type();
        return;

      case TYPE::U_TYPE:
        execute_U_type();
        return;

      case TYPE::J_TYPE:
        execute_J_type();
        return;

      case TYPE::UNKNOWN:
        return;
    }
  }
  
  void CPU::writePC(uint32_t data) 
  {
    // Write to the PC for branch and jump instructions
    program_counter = data;
  }

  void CPU::incrementPC(void) 
  {
    program_counter += 4; 
  }

  uint32_t CPU::readPC(void) const
  { 
    return program_counter; 
  }

  void CPU::writeReg(int address, uint32_t data) 
  {
    register_file.Write(address, data);
  }

  uint32_t CPU::readReg(int address) const 
  { 
    return register_file.Read(address); 
  }



