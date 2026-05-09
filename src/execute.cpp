#include "headers/cpu.hpp"
#include <cmath>
#include <cstdint>

  void CPU::execute_R_type()
  {
    // All R-type instructions operate on two operands from the register file specified at location rs1 and rs2
    // result is written to register file at location rd
    auto rs1 = static_cast<int32_t>((readReg(instruction_fields.rs1)));
    auto rs2 = static_cast<int32_t>((readReg(instruction_fields.rs2)));
    auto &rd = instruction_fields.rd;
    int32_t result {};   

    switch (instruction_fields.Operation) 
    {
      case OPERATION::ADD:
        result = rs1 + rs2;
        break;

      case OPERATION::SUB:
        result = rs1 - rs2;
        break;
      
      case OPERATION::OR:
        result = rs1 | rs2;
        break;
      
      case OPERATION::XOR:
        result = rs1 ^ rs2;
        break;
      
      case OPERATION::AND:
        result = rs1 & rs2;
        break;

      case OPERATION::SLL:
      {
        // Shift left logical (fill leading bits with 0)
        // Shift amount = first 5 bits of rs2 (unsigned since cannot shift by negative amount)
        auto shamt = static_cast<uint32_t>(rs2 & 0x1F);
        result = rs1 << shamt;
        break;
      }

      case OPERATION::SRL:
      {
        // Shift right logical (fill leading bits with 0)
        // Shift amount = first 5 bits of rs2 (unsigned) 
        auto shamt = static_cast<uint32_t>(rs2 & 0x1F);
        // Cast as unsigned because right shifitng signed values in c++ sign extends them (shifts arithmetically) 
        result = static_cast<uint32_t>(rs1) >> shamt;  
        break;  
      }

      case OPERATION::SRA:
      {
        // Shift right arithmetic (fill leading bits with 0)
        // Shift amount = first 5 bits of rs2 (unsigned)
        auto shamt = static_cast<uint32_t>(rs2 & 0x1F);
        result = rs1 >> shamt;  
        break;  
      }

      case OPERATION::SLT:
      {
        if (rs1 < rs2)
          result = 1;
        else
          result = 0;
        break;
      }

      case OPERATION::SLTU:
      {
        if (static_cast<uint32_t>(rs1) < static_cast<uint32_t>(rs2))
          result = 1;
        else
          result = 0;
        break;
      }

      case OPERATION::MUL:
        result = (static_cast<int64_t>(rs1) * static_cast<int64_t>(rs2));
        break;
      
      case OPERATION::MULH:
        // Store bits[63:32] of the result
        result = (static_cast<int64_t>(rs1) * static_cast<int64_t>(rs2)) >> 32;
        break;
      
      case OPERATION::MULHU: 
      {
        auto rs1_u = static_cast<uint32_t>(rs1);
        auto rs2_u = static_cast<uint32_t>(rs2);
        result = (static_cast<uint64_t>(rs1_u) * static_cast<uint64_t>(rs2_u)) >> 32; 
        break;
      }
      
      case OPERATION::MULHSU:
      {
        auto rs2_u = static_cast<uint32_t>(rs2);
        result = (static_cast<int64_t>(rs1) * static_cast<uint64_t>(rs2_u)) >> 32;
        break;
      }

      case OPERATION::DIV:
      {  
        if (rs2 == 0) { 
          result = -1; break;
        }
        // Division overflow
        else if ((rs1 == -2147483648) && (rs2 == -1)) {
          result = rs1; break;
        } 

        result = rs1 / rs2; break;
      }  

      case OPERATION::DIVU:
      {
        if (rs2 == 0) {
          result = -1; break;
        }
        result = static_cast<uint32_t>(rs1) / static_cast<uint32_t>(rs2);
        break;
      }

      case OPERATION::REM:
      {
        if (rs2 == 0) {
          result = rs1; break;
        }
        // Division overflow
        else if ((rs1 == -2147483648) && (rs2 == -1)) {
          result = 0; break;
        } 
        
        result = rs1 % rs2; break;
      }

      case OPERATION::REMU:
      {
        if (rs2 == 0) {
          result = rs1; break;
        }
        result = static_cast<uint32_t>(rs1) % static_cast<uint32_t>(rs2);
        break;
      }
      
      default: 
        break; 
    }

    // Write result back to register rd
    writeReg(rd, result);
    incrementPC();
  }

  void CPU::execute_I_type()
  {
    // All I-type instructions operate on immediate and an operand specified at location rs1 of registerfile
    //   - Except load instructions
    // result is written to register file at location rd
    auto rs1 = static_cast<int32_t>(readReg(instruction_fields.rs1));
    auto &imm = instruction_fields.imm;
    auto &rd = instruction_fields.rd;
    int32_t result {};
      
    switch (instruction_fields.Operation) 
    {
      
      case OPERATION::LB:
        // Cast as signed so it gets sign extended 
        result = static_cast<int8_t>(Data_Memory->Read_Byte(imm + rs1));
        break;

      case OPERATION::LH:
        result = static_cast<int16_t>(Data_Memory->Read_halfWord(imm + rs1));
        break;

      case OPERATION::LW:
        result = Data_Memory->Read_Word(imm + rs1);
        break;

      case OPERATION::LBU:
        result = Data_Memory->Read_Byte(imm + rs1);
        break;

      case OPERATION::LHU:
        result = Data_Memory->Read_halfWord(imm + rs1);
        break;

      
      case OPERATION::ADDI:
        result = rs1 + imm;
        break;
      
      case OPERATION::ORI:
        result = rs1 | imm;
        break;

      case OPERATION::XORI:
        result = rs1 ^ imm;
        break;

      case OPERATION::ANDI:
        result = rs1 & imm;
        break;

      case OPERATION::SLLI:
      {
        // Shift left logical (fill leading bits with 0)
        // Shift amount = first 5 bits of imm (unsigned since cannot shift by negative amount)
        auto shamt = static_cast<uint32_t>(imm & 0x1F);
        result = rs1 << shamt;
        break;
      }

      case OPERATION::SRLI:
      {
        // Shift right logical (fill leading bits with 0)
        // Shift amount = first 5 bits of imm (unsigned) 
        auto shamt = static_cast<uint32_t>(imm & 0x1F);
        // Cast as unsigned because right shifitng signed values in c++ sign extends them (shifts arithmetically) 
        result = static_cast<uint32_t>(rs1) >> shamt;  
        break;  
      }

      case OPERATION::SRAI:
      {
        // Shift right arithmetic (fill leading bits with 0)
        // Shift amount = first 5 bits of imm (unsigned)
        auto shamt = static_cast<uint32_t>(imm & 0x1F);
        result = rs1 >> shamt;  
        break;  
      }

      case OPERATION::SLTI:
      {
        if (rs1 < imm)
          result = 1;
        else
          result = 0;
        break;
      }

      case OPERATION::SLTIU:
      {
        if (static_cast<uint32_t>(rs1) < static_cast<uint32_t>(imm))
          result = 1;
        else
          result = 0;
        break;
      }

      case OPERATION::JALR:
      {
        result = readPC() + 4;
        writeReg(rd, result);

        auto jump_target_address = static_cast<int32_t>(readPC()) + rs1 + imm;
        writePC(jump_target_address);    
        return;
      }

      default: 
        break; 

    }

    // Write result back to register rd
    writeReg(rd, result);
    // Increment PC to next instruction
    incrementPC();
    return;
  }

  void CPU::execute_S_type()
  {
    // Memory address to store value in rs2 = value in rs1 + imm
    uint32_t address = readReg(instruction_fields.rs1) + instruction_fields.imm;
    uint32_t rs2 = readReg(instruction_fields.rs2);

    switch (instruction_fields.Operation) 
    {
            
      case OPERATION::SB:
        // Store first 2 bytes of rs2
        Data_Memory->Write(static_cast<uint8_t>(rs2), address);
        break;

      case OPERATION::SH:
        // Store first 4 bytes of rs2
        Data_Memory->Write(static_cast<uint16_t>(rs2), address);
        break;

      case OPERATION::SW:
        // Store rs2
        Data_Memory->Write(rs2, address);
        break;

       default: 
        break; 
    } 

    incrementPC();     
  }

  void CPU::execute_B_type()
  {
    auto rs1 = static_cast<int32_t>(readReg(instruction_fields.rs1));
    auto rs2 = static_cast<int32_t>(readReg(instruction_fields.rs2));
    auto branch_target_address = instruction_fields.imm + static_cast<int32_t>(readPC());

    switch (instruction_fields.Operation) 
    {
      case OPERATION::BEQ:
        if (rs1 == rs2)
        {
          writePC(branch_target_address);
          return;
        }
        break;

      case OPERATION::BNE:
        if (rs1 != rs2)
        {
          writePC(branch_target_address);
          return;
        }
        break;
      
      case OPERATION::BLT:
        if (rs1 < rs2)
        {
          writePC(branch_target_address);
          return;
        }
        break;

      case OPERATION::BGE:
        if (rs1 >= rs2)
        {
          writePC(branch_target_address);
          return;
        }
        break;

      case OPERATION::BLTU:  
        if (static_cast<uint32_t>(rs1) < static_cast<uint32_t>(rs2))
        {
          writePC(branch_target_address);
          return;
        }
        break;
      
      case OPERATION::BGEU:
        if (static_cast<uint32_t>(rs1) >= static_cast<uint32_t>(rs2))
        {
          writePC(branch_target_address);
          return;
        }
        break;
      
      default:
        break;
    }
    // If branch not taken, pc += 4
    incrementPC();
  }

  void CPU::execute_U_type()
  {
    // U-type instructions load the immediate into upper bits of the destination register
    // leaving lower 12 bits as 0
    auto imm = instruction_fields.imm << 12;

    switch (instruction_fields.Operation) 
    {
      case OPERATION::LUI:
        writeReg(instruction_fields.rd, imm);
        break;

      case OPERATION::AUIPC:
        writeReg(instruction_fields.rd, imm + readPC());
        break;

      default:
        break;
    }
  }

  void CPU::execute_J_type()
  {
    auto &rd = instruction_fields.rd;

    if (instruction_fields.Operation == OPERATION::JAL) 
    {
      // Store address of next instruction  
      writeReg(rd, readPC() + 4);
      
      // Jump to specified instruction
      writePC(instruction_fields.imm + static_cast<int32_t>(readPC()));
    }
  }