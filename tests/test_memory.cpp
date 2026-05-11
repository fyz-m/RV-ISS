#include "gtest/gtest.h"
#include <cstdint>
#include "../src/headers/memory.hpp"

class Registerfile_test : public ::testing::Test 
{
  protected:

    void SetUp() override 
    {
     rf = new RegisterFile(32);
    }

    void TearDown() override
    {
      delete rf;
    }

    RegisterFile* rf;
};

TEST_F(Registerfile_test, can_write_and_read_registers)
{
  rf->Write(5, 50);
  ASSERT_EQ(50, rf->Read(5));

  rf->Write(5, 0);
  ASSERT_EQ(0, rf->Read(5));

  rf->Write(31, 0xFFFFFFFF);
  ASSERT_EQ(0xFFFFFFFF, rf->Read(31));
}

TEST_F(Registerfile_test, register0_is_wired_to_zero)
{
  ASSERT_EQ(0, rf->Read(0));
}

TEST_F(Registerfile_test, writes_to_reg0_are_ignored)
{
  rf->Write(0, 500);
  ASSERT_EQ(0, rf->Read(0));

  rf->Write(0, -1);
  ASSERT_EQ(0, rf->Read(0));
}

TEST_F(Registerfile_test, throws_exception_on_invalid_register_index)
{
  ASSERT_ANY_THROW(rf->Write(-1, 0));
  ASSERT_ANY_THROW(rf->Write(32, 0));

  ASSERT_ANY_THROW(rf->Read(-1));
  ASSERT_ANY_THROW(rf->Read(32));
}

class MemoryTest : public ::testing::Test 
{
  protected:

    void SetUp() override 
    {
     memory = new Memory(128);
    }

    void TearDown() override
    {
      delete memory;
    }

    Memory* memory;
};

TEST_F(MemoryTest, can_read_and_write_bytes)
{
  memory->Write(static_cast<uint8_t>(0xAB), 1);
  ASSERT_EQ(memory->Read_Byte(1), 0xAB);

  memory->Write(static_cast<uint8_t>(0xFFFFFFFF), 2);
  ASSERT_EQ(memory->Read_Byte(2), 0xFF);
}

TEST_F(MemoryTest, can_read_and_write_words)
{
  memory->Write(static_cast<uint32_t>(0xDEADBEEF), 1);
  ASSERT_EQ(memory->Read_Word(1), 0xDEADBEEF);
  ASSERT_EQ(memory->Read_Byte(1), 0xEF);
  ASSERT_EQ(memory->Read_halfWord(1), 0xBEEF);

  memory->Write(static_cast<uint32_t>(0xFFFFFFFF), 4);
  ASSERT_EQ(memory->Read_Word(4), 0xFFFFFFFF);
}

TEST_F(MemoryTest, can_read_and_write_halfword)
{
  memory->Write(static_cast<uint16_t>(0xAB), 1);
  ASSERT_EQ(memory->Read_halfWord(1), 0x00AB);

  memory->Write(static_cast<uint16_t>(0xFFFFFFFF), 1);
  ASSERT_EQ(memory->Read_halfWord(1), 0xFFFF);
}

TEST_F(MemoryTest, resizes_on_addresses_greater_than_current_length)
{
  memory->Write(static_cast<uint32_t>(0xABCDEF00), 4000);
  ASSERT_EQ(memory->Read_Word(4000), 0xABCDEF00);
  ASSERT_EQ(memory->Read_halfWord(4000), 0xEF00);
  ASSERT_EQ(memory->Read_Byte(4000), 0x00);
}

TEST_F(MemoryTest, throws_exception_on_invalid_reads)
{
  // size = 0
  ASSERT_ANY_THROW(memory->Read_Byte(0));
  ASSERT_ANY_THROW(memory->Read_Byte(1));
  
  // size = 2
  memory->Write(static_cast<uint8_t>(0xA), 1);
  ASSERT_NO_FATAL_FAILURE(memory->Read_Byte(1));
  ASSERT_NO_FATAL_FAILURE(memory->Read_Byte(0));
  ASSERT_NO_FATAL_FAILURE(memory->Read_halfWord(0));
  ASSERT_ANY_THROW(memory->Read_Byte(2));
  ASSERT_ANY_THROW(memory->Read_Word(0));
}

TEST_F(MemoryTest, max_resize)
{
  // Memory object will take up 4 GB of heap storage
  memory->Write(static_cast<uint32_t>(0x11), 0xFFFFFFFF);
  ASSERT_EQ(memory->Read_Byte(0xFFFFFFFF), 0x11);
}