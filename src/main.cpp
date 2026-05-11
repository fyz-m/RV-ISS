#include "headers/cpu.hpp"
#include "headers/memory.hpp"
#include <iostream>
#include <memory>
#include <ostream>
#include <string_view>


constexpr int WIDTH = 32;

int main(int argc, char* argv[]) {

  if (argc != 3) {
    std::cerr << "\tUsage: ./rv_iss binaryfile.bin p=true/false" << std::endl;
    return 1;
  }
  
  bool print_state {false};
  std::string_view p{argv[2]};
  if (p == "p=true")
    print_state = true;


  auto Instruction_Memory = std::make_shared <Memory>();
  auto Data_Memory = std::make_shared <Memory>();

  auto program = argv[1];
  if (!Instruction_Memory->Load(program))
      return 1;
  
  CPU cpu (WIDTH, Instruction_Memory, Data_Memory);
  cpu.runtoEbreak();

  std::string_view input_p { program};
  if (input_p  ==  "tests/programs/factorial_12.bin") {
    std::cout << "Calculated 12! in " << cpu.cycles() << " cycles" << std::endl;
    std::cout << "Register a0 = " << cpu.readReg(10) << std::endl;
  }

  if (print_state) 
    cpu.printState();
  
} 

