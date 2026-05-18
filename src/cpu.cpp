#include "headers/cpu.hpp"
#include "headers/memory.hpp"
#include "headers/decode.hpp"
#include <cstdint>
#include <iostream>
#include <memory>
#include <stdexcept>

  CPU::CPU(int width, 
           std::shared_ptr<Memory> Instruction_Memory_ptr, 
           std::shared_ptr<Memory> Data_Memory_ptr) 
           
    : instruction_register{}, instruction_fields{}, program_counter{},
      register_file(width), 
      Instruction_Memory(Instruction_Memory_ptr), 
      Data_Memory(Data_Memory_ptr)
    {}

  void CPU::runtoEbreak(const int MAX_CYCLES)
  {
    while(cycle_count < MAX_CYCLES)
    {
      Fetch();  
      if (instruction_register == EBREAK_ENCODING)
          return;
      Decode();
      Execute();
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

    if (instruction_fields.type == TYPE::UNKNOWN) {
      std::cerr << "      " << instruction_fields.raw_inst << std::endl;
      throw std::runtime_error("Unable to decode type ^^");
    }

    if (instruction_fields.Operation == OPERATION::UNKNOWN) {
      std::cerr << "      " << instruction_fields.raw_inst << std::endl;
      throw std::runtime_error("Unable to decode operation ^^");
    }
    
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
  
  void CPU::writePC(const uint32_t &data) 
  {
    // Write to the PC for branch and jump instructions
    program_counter = data;
  }

  void CPU::incrementPC() 
  {
    program_counter += 4; 
  }

  uint32_t CPU::readPC() const
  { 
    return program_counter; 
  }

  void CPU::writeReg(const int &address, const uint32_t &data) 
  {
    register_file.Write(address, data);
  }

  uint32_t CPU::readReg(const int &address) const 
  { 
    return register_file.Read(address); 
  }

  int CPU::cycles()
  {
    return cycle_count;
  }
  
  std::shared_ptr<Memory> CPU::getInstructionMemory() const
  {
    return Instruction_Memory;
  } 
  
  std::shared_ptr<Memory> CPU::getDataMemory() const
  {
    return Data_Memory;
  }

  void CPU::printState()
  {
    std::cout << '\n';
    std::cout << "---------------------------- ARCHITECTURAL STATE ----------------------------" << std::endl;
    std::cout << '\n';
    std::cout << "Program Counter: " << CPU::program_counter <<std::endl;
    // Instruction register is not part of Architectural state but is useful 
    std::cout << "Instruction Register : " << CPU::instruction_register <<std::endl;
    std::cout << '\n';

    for (int i = 0; i < 32; i++)
    {
      std::cout << "Register " << i << " : " << CPU::readReg(i) << std::endl;
    }
  }
