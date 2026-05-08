#include "headers/decode.hpp"
#include "headers/cpu.hpp"
#include <cstdint>
#include <iostream>


void decode(DecodedInstruction &input_instruction) 
{
  
  input_instruction.opcode = input_instruction.raw_inst & 0x0000007F;
  set_type(input_instruction);

  switch (input_instruction.type) {

  case TYPE::R_TYPE:

    decode_R_type(input_instruction);
    break;

  case TYPE::I_TYPE:

    decode_I_type(input_instruction);
    break;

  case TYPE::S_TYPE:

    decode_S_type(input_instruction);
    break;

  case TYPE::B_TYPE:

    decode_B_type(input_instruction);
    break;

  case TYPE::U_TYPE:

    decode_U_type(input_instruction);
    break;

  case TYPE::J_TYPE:

    decode_J_type(input_instruction);
    break;

  default:
    std::cerr << "Unable to resolve instruction type" << std::endl;
  }
}

void decode_R_type(DecodedInstruction& fields) 
{
  // extract fields
  extract_R_type(fields);

  // Set operation
  switch (fields.funct3) 
  {
    case 0:

      switch (fields.funct7) 
      {
        case 1:
          fields.Operation = OPERATION::MUL; return;    
        case 0:
          fields.Operation = OPERATION::ADD; return;
        case 32:
          fields.Operation = OPERATION::SUB; return;
      } break;
      
    case 1:
      
      switch (fields.funct7) 
      {
        case 0:
          fields.Operation = OPERATION::SLL; return;    
        case 1:
          fields.Operation = OPERATION::MULH; return;
      } break;

    case 2:
      
      switch (fields.funct7) 
      {
        case 0:
          fields.Operation = OPERATION::SLT; return;    
        case 1:
          fields.Operation = OPERATION::MULSU; return;
      } break;

    case 3:
      
      switch (fields.funct7) 
      {
        case 0:
          fields.Operation = OPERATION::SLTU; return;    
        case 1:
          fields.Operation = OPERATION::MULU; return;
      } break;

    case 4:
      
      switch (fields.funct7) 
      {
        case 0:
          fields.Operation = OPERATION::XOR; return;    
        case 1:
          fields.Operation = OPERATION::DIV; return;
      } break;

    case 5:

      switch (fields.funct7) 
      {
        case 0:
          fields.Operation = OPERATION::SRL; return;    
        case 1:
          fields.Operation = OPERATION::DIVU; return;
        case 32:
          fields.Operation = OPERATION::SRA; return;
      } break;

    case 6:
      
      switch (fields.funct7) 
      {
        case 0:
          fields.Operation = OPERATION::OR; return;    
        case 1:
          fields.Operation = OPERATION::REM; return;
      } break;

    case 7:
        
      switch (fields.funct7) 
      {
        case 0:
          fields.Operation = OPERATION::AND; return;    
        case 1:
          fields.Operation = OPERATION::REMU; return;
      } break;
      
  }
  std::cerr << "Unable to find operation" << std::endl;
}

void decode_I_type(DecodedInstruction& fields) 
{

  extract_I_type(fields);

  switch (fields.funct3) 
  {
    case 0:
      fields.Operation = (fields.opcode == 19) ? OPERATION::ADDI : OPERATION::LB; 
      break;   
    case 1:
      fields.Operation = (fields.opcode == 19) ? OPERATION::SLLI : OPERATION::LH; 
      break;
    case 2:
      fields.Operation = (fields.opcode == 19) ? OPERATION::SLTI : OPERATION::LW; 
      break;
    case 3:
      fields.Operation =  OPERATION::SLTIU; 
      break;
    case 4:
      fields.Operation = (fields.opcode == 19) ? OPERATION::XORI : OPERATION::LBU; 
      break;
    case 5:
    {
      if (fields.opcode == 3)
      {
        fields.Operation = OPERATION::LHU;
        break;
      }
      // srli and srai have the same funct3 and opcode so are differentiated by imm[10] being set
      // Instruction[31:25] are redudant in shift instructions so is used as funct7
      fields.funct7 = fields.raw_inst >> 25;
      fields.Operation = (fields.funct7 == 0) ? OPERATION::SRLI : OPERATION::SRAI;  
      break;
    }
    case 6:
      fields.Operation = OPERATION::ORI;
      break;
    case 7:
      fields.Operation = OPERATION::ANDI;
      break;
    default:
        fields.Operation = (fields.opcode == 103) ? OPERATION::JALR : OPERATION::UNKNOWN;
      
  }

  if (fields.Operation == OPERATION::SRLI || fields.Operation == OPERATION::SRAI || fields.Operation == OPERATION::SLLI)
    // Only first 5 bits of immediate are used for shift instruction 
    // because register width = 32 so upper 7 bits of imm are redundant
    fields.imm &= 0x1F;
}

void decode_S_type(DecodedInstruction& fields) 
{

  extract_S_type(fields);

  switch (fields.funct3) 
  {
    case 0:
      fields.Operation = OPERATION::SB;
      break;
    case 1:
      fields.Operation = OPERATION::SH;
      break; 
    case 2:
      fields.Operation = OPERATION::SW;
      break;
  }
}

void decode_B_type(DecodedInstruction& fields) 
{

  extract_B_type(fields);

  switch (fields.funct3) 
  {
    case 0:
      fields.Operation = OPERATION::BEQ; 
      break;   
    case 1:
      fields.Operation = OPERATION::BNE; 
      break;
    case 4:
      fields.Operation = OPERATION::BLT; 
      break;
    case 5:
      fields.Operation = OPERATION::BGE; 
      break;
    case 6:
      fields.Operation = OPERATION::BLTU; 
      break;
    case 7:
      fields.Operation = OPERATION::BGEU; 
  }
}

void decode_U_type(DecodedInstruction& fields) 
{

  extract_U_type(fields);

  fields.Operation = (fields.opcode == 55) ? OPERATION::LUI : OPERATION::AUIPC; 
}

void decode_J_type(DecodedInstruction& fields) 
{

  extract_J_type(fields);

  fields.Operation =  (fields.opcode == 111) ? OPERATION::JAL : OPERATION::UNKNOWN; 
}

void extract_R_type(DecodedInstruction& fields)
{
  extract_rd(fields);
  extract_funct3(fields);
  extract_rs1(fields);
  extract_rs2(fields);
  extract_funct7(fields);
}

void extract_I_type(DecodedInstruction& fields)
{
  extract_rd(fields);
  extract_funct3(fields);
  extract_rs1(fields);
  fields.imm = static_cast<int32_t>(fields.raw_inst) >> 20;
}

void extract_S_type(DecodedInstruction& fields)
{
  extract_funct3(fields);
  extract_rs1(fields);
  extract_rs2(fields);

  // Imm[4:0]
  auto imm_4_0 = (fields.raw_inst >> 7) & 0x1F;

  // Imm[11:5]
  // Cast as signed so shift is arithmetic (sign extends)
  auto imm_11_5 = static_cast<int32_t>(fields.raw_inst) >> 25;
  
  fields.imm = (imm_11_5 << 5) | (imm_4_0);
}

void extract_B_type(DecodedInstruction& fields)
{
  extract_funct3(fields);
  extract_rs1(fields);
  extract_rs2(fields);
  
  // Imm[11]
  auto imm_11 = (fields.raw_inst >> 7) & 0x1;

  // Imm[4:1]
  // Bit 0 is not encoded because branch offsets (n.o bytes to instruction to jump to) are always a multiple of 4
  // Because each RISC-V instructions are 32-bit (4-bytes)
  auto imm_4_1 = (fields.raw_inst >> 8) & 0xF;

  // Imm[10:5]
  auto imm_10_5 = (fields.raw_inst >> 25) & 0x3F;

  // Imm[12]
  // Cast for sign extension
  auto imm_12 = static_cast<int32_t>(fields.raw_inst) >> 31;

  fields.imm = (imm_12 << 12) | (imm_11 << 11) | (imm_10_5 << 5) | (imm_4_1 << 1) | 0b0;
}

void extract_U_type(DecodedInstruction& fields)
{
  extract_rd(fields);
  fields.imm = static_cast<int32_t>(fields.raw_inst) >> 12;
}

void extract_J_type(DecodedInstruction& fields)
{
  extract_rd(fields);

  // imm[19:12]
  auto imm_19_12 = (fields.raw_inst >> 12) & 0xFF;

  // imm[11]
  auto imm_11 = (fields.raw_inst >> 20) & 0x1;

  // imm[10:1]
  auto imm_10_1 = (fields.raw_inst >> 21) & 0x3FF;

  // imm[20]
  auto imm_20 = static_cast<int32_t>(fields.raw_inst) >> 31;

  fields.imm = (imm_20 << 20) | (imm_19_12 << 12) | (imm_11 << 11) | (imm_10_1 << 1) | 0b0;
}

void extract_rd(DecodedInstruction& fields) 
{
  // Extract the destination register from an instruction
  fields.rd =  (fields.raw_inst >> 7) & 0x1F;
}

void extract_rs1(DecodedInstruction& fields) 
{
  // Extract the first source register from an instruction
  fields.rs1 = (fields.raw_inst >> 15) & 0x1F;
}

void extract_rs2(DecodedInstruction& fields) 
{
  // Extract the second source register from an instruction
  fields.rs2 = (fields.raw_inst >> 20) & 0x1F;
}

void extract_funct3(DecodedInstruction& fields) 
{
  // Extract the 3-bit function field from an instruction
  fields.funct3 = (fields.raw_inst >> 12) & 0x7;
}

void extract_funct7(DecodedInstruction& fields) 
{
  // Extract the 7-bit function field from an instruction
  fields.funct7 = fields.raw_inst >> 25;
}

void set_type(DecodedInstruction& fields)
{
  switch (fields.opcode) {
  case 51:
    fields.type = TYPE::R_TYPE; 
    break;

  case 19:
  case 3:
  case 103:
    fields.type = TYPE::I_TYPE;
    break;

  case 35:
    fields.type = TYPE::S_TYPE;
    break;

  case 99:
    fields.type = TYPE::B_TYPE;
    break;

  case 55:
  case 23:
    fields.type = TYPE::U_TYPE;
    break;

  case 111:
    fields.type = TYPE::J_TYPE;
    break;

  default:
    std::cerr << "Unable to resolve instruction type" << std::endl;
  }
}