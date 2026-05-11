#include "headers/memory.hpp"
#include <cstdint>
#include <fstream>
#include <iostream>
#include <iterator>
#include <ostream>
#include <stdexcept>

// std::bitset to limit the address to 5-bits can limit the index so it is never out of bounds
// Could increase perfomance as it makes validate_address() redundant  

  RegisterFile::RegisterFile(int width)
      : registers{0},  // in RISC-V, register x0 is hardwired to 0
        WIDTH(width)
        {}

  void RegisterFile::Write(const int &address, const uint32_t &data) 
  {
    validate_address(address);
    // Register x0 is hardwired to 0 so writes to the first register are ignored
    if (address == 0)
      return;

    registers[address] = data;
  }

  uint32_t RegisterFile::Read(const int &address) const 
  { 
    validate_address(address);
    return registers[address]; 
  }

  void RegisterFile::validate_address(const int& address) const
  {
    if ( !(0 <= address && address < REGISTER_FILE_SIZE) )
      throw std::out_of_range("Invalid register index");
  }



  
  Memory::Memory(int size)     
  {
    // Pre-allocate memory of amount 'size' to the array
    // reserve() changes the capacity of array but not size
    // So if resize() is called (in Write() member method),
    // the array will not have to be re-allocated (very expensive) as long as capacity <= size
    m_memory.reserve(size); 
  }

  void Memory::Write(const uint8_t &data, const std::size_t &address) 
  {
    // Write a byte at address

    // If address is greater than current length, resize vector
    if (address >= std::size(m_memory)) 
        m_memory.resize(address + 1);
    
    m_memory[address] = data;
  }

  void Memory::Write(const uint32_t &data, const std::size_t &address) 
  {
    // Write a word (4 bytes)
    for (int i = 0; i < 4; i++) 
    {
      uint8_t data_byte = data >> (i * 8);
      Write(data_byte, address + i);
    }
  }

  void Memory::Write(const uint16_t &data, const std::size_t &address)
  {
    // Write a half-word (2 bytes)
    for (int i = 0; i < 2; i++) 
    {
      // Extract byte to write
      uint8_t data_byte = data >> (i * 8);
      Write(data_byte, address + i);
    }
  }

  uint8_t Memory::Read_Byte(const std::size_t &address) const 
  { 
    if (address >= std::size(m_memory))
        throw std::runtime_error("Out of bounds read");

    return m_memory[address]; 
  }

  uint32_t Memory::Read_Word(const std::size_t &address) const 
  {
    uint32_t word{};

    for (int i = 0; i < 4; i++) {
      auto byte = static_cast<uint32_t>(Read_Byte(address + i));

      word |= byte << (8 * i);
    }
    return word;
  }

  uint16_t Memory::Read_halfWord(const std::size_t &address) const
  {
   uint32_t word{};

    for (int i = 0; i < 2; i++) {
      auto byte = static_cast<uint32_t>(Read_Byte(address + i));

      word |= byte << (8 * i);
    }
    return word;
  }

  bool Memory::Load(const char *filepath) 
  {
    // Load contents of a binary file into the memory
    std::ifstream input_file{filepath, std::ios::binary | std::ios::in};

    if (!input_file) 
    {
      std::cout << "Could not open file" << std::endl;
      return false;
    }

    uint8_t inst_byte {};
    while (input_file.read(reinterpret_cast<char*>(&inst_byte), sizeof(uint8_t))) 
    {
      m_memory.push_back(inst_byte);
    }
    input_file.close();
    return true;
  }

  