#pragma once
#include <array>
#include <cstddef>
#include <cstdint>
#include <vector>

constexpr int REGISTER_FILE_SIZE = 32;

class RegisterFile {

  private:

    std::array<uint32_t, REGISTER_FILE_SIZE> registers{};

    int WIDTH;
    
  public:

    RegisterFile(int width = 32);

    void Write(const int &address, const uint32_t &data);

    uint32_t Read(const int &address) const;
  
  private:

    void validate_address(const int& address) const;

};

class Memory {

private:
  // RISC-V uses byte-addressable memory, so memory must be an array of bytes
  std::vector<uint8_t> m_memory{};

public:

  Memory(int size = 2048);

  void Write(const uint8_t &data, const std::size_t &address);

  void Write(const uint32_t &data, const std::size_t &address);

  void Write(const uint16_t &data, const std::size_t &address);

  // Returns the byte at the given address 
  uint8_t Read_Byte(const std::size_t &address) const;

  // Returns the word (8 bytes) at the given address 
  // A word is 32-bit in RISC-V, even in RV64  
  uint32_t Read_Word(const std::size_t &address) const;

  // Returns the half word (4 bytes) at the given address 
  uint16_t Read_halfWord(const std::size_t &address) const;

  // Load a program into memory
  bool Load(const char *filepath);

};


