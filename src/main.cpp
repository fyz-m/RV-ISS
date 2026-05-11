#include "headers/cpu.hpp"
#include "headers/memory.hpp"
#include <iostream>
#include <memory>
#include <ostream>


constexpr int WIDTH = 32;

int main(int argc, char* argv[]) {

  if (argc != 2) {
    std::cerr << "\tUsage: ./rv_iss binaryfile.bin" << std::endl;
    return 1;
  }
  auto program = argv[1];

  auto Instruction_Memory = std::make_shared <Memory>();
  auto Data_Memory = std::make_shared <Memory>();

  if (!Instruction_Memory->Load(program))
      return 1;
  
  CPU cpu (WIDTH, Instruction_Memory, Data_Memory);
  cpu.runtoEbreak();

  std::string_view input_p { program};
  if (input_p  ==  "tests/programs/factorial_12.bin")
  {
    std::cout << "Calculated 12! in " << cpu.cycles() << " cycles" << std::endl;
    std::cout << "Register a0 = " << cpu.readReg(10) << std::endl;
  }

} 

