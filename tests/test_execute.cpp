#include "gtest/gtest.h"
#include <cstdint>
#include <memory>
#include <sys/socket.h>
#include "../src/headers/cpu.hpp"


struct Rtype_Execute_Case {
    OPERATION operation;
    int32_t rs1_value; // value of operand 1
    int32_t rs2_value; // value of operand 2
    int32_t expected_rd_value; // expected value
    std::string test_name;
};

class Rtype_Execute_Test : public ::testing::TestWithParam<Rtype_Execute_Case> {};

TEST_P(Rtype_Execute_Test, executes)
{
    Rtype_Execute_Case tc = GetParam();

    CPU_test cpu;
    cpu.instruction_fields.type = TYPE::R_TYPE;
    cpu.instruction_fields.Operation = tc.operation;
    cpu.instruction_fields.rs1 = 1;
    cpu.instruction_fields.rs2 = 2;
    cpu.instruction_fields.rd = 3;

    cpu.writeReg(1, tc.rs1_value);
    cpu.writeReg(2, tc.rs2_value);

    cpu.Execute();
    auto rd = static_cast<int32_t>(cpu.readReg(3));
    EXPECT_EQ(rd, tc.expected_rd_value);

}

INSTANTIATE_TEST_SUITE_P(R_TYPE, Rtype_Execute_Test,

    ::testing::Values(
      Rtype_Execute_Case{OPERATION::ADD, 25,  25, 50, "add_basic"},
      Rtype_Execute_Case{OPERATION::ADD, 0,  -1, -1, "add_negative_number"},
      Rtype_Execute_Case{OPERATION::ADD, -300,  -700, -1000, "add_negative_numbers"},
      Rtype_Execute_Case{OPERATION::ADD,  2147483647, 1, -2147483648, "add_positive_overflow"},
      Rtype_Execute_Case{OPERATION::ADD,  -2147483648, -1, 2147483647, "add_negative_overflow"},

      Rtype_Execute_Case{OPERATION::SUB, 100,  25, 75, "sub_basic"},
      Rtype_Execute_Case{OPERATION::SUB, 200,  -200, 400, "sub_negative_number"},
      Rtype_Execute_Case{OPERATION::SUB, -300,  -700, 400, "sub_negative_numbers"},
      Rtype_Execute_Case{OPERATION::SUB,  2147483647, -1, -2147483648, "sub_positive_overflow"},
      Rtype_Execute_Case{OPERATION::SUB,  -2147483648, 1, 2147483647, "sub_negative_overflow"},

      Rtype_Execute_Case{OPERATION::XOR, -1, -1, 0, "xor_case"},
      Rtype_Execute_Case{OPERATION::OR, -1, -0, -1, "or_case"},
      Rtype_Execute_Case{OPERATION::AND, -1, -1, -1, "and_case"},
      Rtype_Execute_Case{OPERATION::AND, -1, 0, 0, "and_case_2"},
      
      Rtype_Execute_Case{OPERATION::SLL, 2, 2, 8, "shift_left_logical_basic"},
      Rtype_Execute_Case{OPERATION::SLL, 1, 1, 2, "shift_left_logical_fills_leading_bits_with_zero"},
      Rtype_Execute_Case{OPERATION::SLL, 2147483647, 0b11111, -2147483648, "shift_left_logical_max"},
      Rtype_Execute_Case{OPERATION::SLL, 1, 0b11111, -2147483648, "shift_left_logical_max_2"},

      Rtype_Execute_Case{OPERATION::SRL, 8, 2, 2, "shift_right_logical_basic"},
      Rtype_Execute_Case{OPERATION::SRL, -1, 1, 0x7FFFFFFF, "shift_right_logical_does_not_signext"},
      Rtype_Execute_Case{OPERATION::SRL, 2147483647, 0b11111, 0, "shift_right_logical_max"},
      Rtype_Execute_Case{OPERATION::SRL, -2147483648, 0b11111, 1, "shift_right_logical_max_2"},

      Rtype_Execute_Case{OPERATION::SRA, 8, 2, 2, "shift_right_arithmetic_basic"},
      Rtype_Execute_Case{OPERATION::SRA, -8, 2, -2, "shift_right_arithmetic_negative"},
      Rtype_Execute_Case{OPERATION::SRA, -1, 3, -1, "shift_right_arithmetic_does_signext"},
      Rtype_Execute_Case{OPERATION::SRA, 2147483647, 0b11111, 0, "shift_right_arithmetic_max"},
      Rtype_Execute_Case{OPERATION::SRA, -2147483648, 0b11111, -1, "shift_right_arithmetic_max_2"},

      Rtype_Execute_Case{OPERATION::SLT, 20, 30, 1, "set_less_than_basic" },
      Rtype_Execute_Case{OPERATION::SLT, 1200, 500, 0, "set_less_than_basic_2"},
      Rtype_Execute_Case{OPERATION::SLT, 1, 1, 0, "set_less_than_equal_values"},
      Rtype_Execute_Case{OPERATION::SLT, -5, 0, 1, "set_less_than_negative_values"},
      Rtype_Execute_Case{OPERATION::SLT, -700, -1000, 0, "set_less_than_negative_values_2"},

      Rtype_Execute_Case{OPERATION::SLTU, 20, 30, 1, "set_less_than_u_basic" },
      Rtype_Execute_Case{OPERATION::SLTU, 1200, 500, 0, "set_less_than_u_basic_2"},
      Rtype_Execute_Case{OPERATION::SLTU, 1, 1, 0, "set_less_than_u_equal_values"},
      Rtype_Execute_Case{OPERATION::SLTU, -1, 0, 0, "set_less_than_u_negative_values"},
      Rtype_Execute_Case{OPERATION::SLTU, 0, -1, 1, "set_less_than_u_negative_values_2"},
      Rtype_Execute_Case{OPERATION::SLTU, 0x7FFFFFFF, static_cast<int32_t>(0x80000000), 1, "set_less_than_u_compares_as_unsigned"},
      Rtype_Execute_Case{OPERATION::SLTU, static_cast<int32_t>(0x80000000), static_cast<int32_t>(0x80000001), 1, "set_less_than_u_compares_as_unsigned_2"},

      Rtype_Execute_Case{OPERATION::MUL, 4, 4, 16, "mul_basic"},
      Rtype_Execute_Case{OPERATION::MUL, 4, -4, -16, "mul_negative"},
      Rtype_Execute_Case{OPERATION::MUL, 0x7FFFFFFF, 50, static_cast<int32_t>(0xFFFFFFCE), "mul_stores_bottom_half_of_result"},
      Rtype_Execute_Case{OPERATION::MUL, 0x7FFFFFFF, -50, 0x00000032, "mul_stores_bottom_half_of_result_negative"},

      Rtype_Execute_Case{OPERATION::MULH, 0x7FFFFFFF, 50, 0x18, "mulh_stores_top_half_of_result"},
      Rtype_Execute_Case{OPERATION::MULH, 0x7FFFFFFF, -50, static_cast<int32_t>(0xFFFFFFE7), "mulh_stores_top_half_of_result_negative"},
      Rtype_Execute_Case{OPERATION::MULH, -1, -1, 1, "mulh_treats_operands_as_signed"},

      Rtype_Execute_Case{OPERATION::MULHU, static_cast<int32_t>(0xFFFFFFFF), 50, 0x32, "mulhu_stores_top_half_of_result"},
    
      // rs1 = signed, rs2 = unsigned
      Rtype_Execute_Case{OPERATION::MULHSU, 2, static_cast<int32_t>(0xFFFFFFFF), 0x2, "mulhu_treats_rs2_as_unsigned"},
      Rtype_Execute_Case{OPERATION::MULHSU, -2, static_cast<int32_t>(0xFFFFFFFF), static_cast<int32_t>(0xFFFFFFFE), "mulhu_treats_rs1_as_signed"},
      
      Rtype_Execute_Case{OPERATION::DIV, 10, 5, 2, "div_basic"},
      Rtype_Execute_Case{OPERATION::DIV, 10, -5, -2, "div_negative"},
      Rtype_Execute_Case{OPERATION::DIV, -10, -5, 2, "div_both_negative"},
      Rtype_Execute_Case{OPERATION::DIV, 10, 0, -1, "div_zero"},  // RISC-V division by 0 sets all bits to 1 
      Rtype_Execute_Case{OPERATION::DIV, 0x10000000, -1, 0x10000000, "div_overflow"}, // Divison overflow sets rd = rs1   

      Rtype_Execute_Case{OPERATION::DIVU, 10, 5, 2, "divu_basic"},
      Rtype_Execute_Case{OPERATION::DIVU, -1, 2, 0x8000000, "divu_treats_operands_as_unsigned"},
      Rtype_Execute_Case{OPERATION::DIVU, 10, 0, -1, "divu_zero"},  
      
      
      Rtype_Execute_Case{OPERATION::REM, 12, 5, 2, "rem_basic"},
      Rtype_Execute_Case{OPERATION::REM, -12, -5, -2, "rem_negative"},
      // for negative operands, sign of remainder (rd) is equal to sign of rs1
      Rtype_Execute_Case{OPERATION::REM, 12, -5, 2, "rem_takes_sign_of_rs1"},
      Rtype_Execute_Case{OPERATION::REM, -15, 10, -5, "rem_takes_sign_of_rs1_2"},
      // RISC-V defines remainder = rs1 if dividing by 0
      Rtype_Execute_Case{OPERATION::REM, 10, 0, 10, "rem_divide_by_0"},
    
      


      Rtype_Execute_Case{OPERATION::REMU, 12, 5, 2, "remu_basic"},
      Rtype_Execute_Case{OPERATION::REMU, 10, 0, 10, "remu_divide_by_0"}

    ),

    [](const ::testing::TestParamInfo<Rtype_Execute_Case>& info) {
      return info.param.test_name;
      }
);

struct Itype_Execute_Case {
    OPERATION operation;
    int32_t rs1_value; // value of operand 1
    int32_t imm; // value of immediate
    int32_t expected_rd_value; // expected value
    std::string test_name;
};

class Itype_Execute_Test : public ::testing::TestWithParam<Itype_Execute_Case> {};

TEST_P(Itype_Execute_Test, executes)
{
    Itype_Execute_Case tc = GetParam();

    CPU_test cpu;
    cpu.instruction_fields.type = TYPE::I_TYPE;
    cpu.instruction_fields.Operation = tc.operation;
    cpu.instruction_fields.rs1 = 1;
    cpu.instruction_fields.imm = tc.imm;
    cpu.instruction_fields.rd = 3;

    cpu.writeReg(1, tc.rs1_value);

    cpu.Execute();
    auto rd = static_cast<int32_t>(cpu.readReg(3));
    EXPECT_EQ(rd, tc.expected_rd_value);

}

INSTANTIATE_TEST_SUITE_P(I_TYPE, Itype_Execute_Test,

    ::testing::Values(
      Itype_Execute_Case{OPERATION::ADDI, 25,  25, 50, "addi_basic"},
      Itype_Execute_Case{OPERATION::ADDI, 0,  -1, -1, "addi_negative_number"},
      Itype_Execute_Case{OPERATION::ADDI, -300,  -700, -1000, "addi_negative_numbers"},
      Itype_Execute_Case{OPERATION::ADDI,  2147483647, 1, -2147483648, "addi_positive_overflow"},
      Itype_Execute_Case{OPERATION::ADDI,  -2147483648, -1, 2147483647, "addi_negative_overflow"},

      Itype_Execute_Case{OPERATION::XORI, -1, -1, 0, "xori_case"},
      Itype_Execute_Case{OPERATION::ORI, -1, -0, -1, "ori_case"},
      Itype_Execute_Case{OPERATION::ANDI, -1, -1, -1, "andi_case"},
      Itype_Execute_Case{OPERATION::ANDI, -1, 0, 0, "andi_case_2"},
      
      Itype_Execute_Case{OPERATION::SLLI, 2, 2, 8, "shift_left_logical_imm_basic"},
      Itype_Execute_Case{OPERATION::SLLI, 1, 1, 2, "shift_left_logical_imm_fills_leading_bits_with_zero"},
      Itype_Execute_Case{OPERATION::SLLI, 2147483647, 0b11111, -2147483648, "shift_left_logical_imm_max"},
      Itype_Execute_Case{OPERATION::SLLI, 1, 0b11111, -2147483648, "shift_left_logical_imm_max_2"},

      Itype_Execute_Case{OPERATION::SRLI, 8, 2, 2, "shift_right_logical_imm_basic"},
      Itype_Execute_Case{OPERATION::SRLI, -1, 1, 0x7FFFFFFF, "shift_right_logical_imm_does_not_signext"},
      Itype_Execute_Case{OPERATION::SRLI, 2147483647, 0b11111, 0, "shift_right_logical_imm_max"},
      Itype_Execute_Case{OPERATION::SRLI, -2147483648, 0b11111, 1, "shift_right_logical_imm_max_2"},

      Itype_Execute_Case{OPERATION::SRAI, 8, 2, 2, "shift_right_arithmetic_imm_basic"},
      Itype_Execute_Case{OPERATION::SRAI, -8, 2, -2, "shift_right_arithmetic_imm_negative"},
      Itype_Execute_Case{OPERATION::SRAI, -1, 3, -1, "shift_right_arithmetic_imm_does_signext"},
      Itype_Execute_Case{OPERATION::SRAI, 2147483647, 0b11111, 0, "shift_right_arithmetic_imm_max"},
      Itype_Execute_Case{OPERATION::SRAI, -2147483648, 0b11111, -1, "shift_right_arithmetic_imm_max_2"},

      Itype_Execute_Case{OPERATION::SLTI, 20, 30, 1, "set_less_than_imm_basic" },
      Itype_Execute_Case{OPERATION::SLTI, 1200, 500, 0, "set_less_than_imm_basic_2"},
      Itype_Execute_Case{OPERATION::SLTI, 1, 1, 0, "set_less_than_imm_equal_values"},
      Itype_Execute_Case{OPERATION::SLTI, -5, 0, 1, "set_less_than_imm_negative_values"},
      Itype_Execute_Case{OPERATION::SLTI, -700, -1000, 0, "set_less_than_imm_negative_values_2"},

      Itype_Execute_Case{OPERATION::SLTIU, 20, 30, 1, "set_less_than_imm_u_basic" },
      Itype_Execute_Case{OPERATION::SLTIU, 1200, 500, 0, "set_less_than_imm_u_basic_2"},
      Itype_Execute_Case{OPERATION::SLTIU, 1, 1, 0, "set_less_than_imm_u_equal_values"},
      Itype_Execute_Case{OPERATION::SLTIU, -1, 0, 0, "set_less_than_imm_u_negative_values"},
      Itype_Execute_Case{OPERATION::SLTIU, 0, -1, 1, "set_less_than_imm_u_negative_values_2"},
      Itype_Execute_Case{OPERATION::SLTIU, 0x7FFFFFFF, -1, 1, "set_less_than_imm_u_compares_as_unsigned"}

    ),

    [](const ::testing::TestParamInfo<Itype_Execute_Case>& info) {
      return info.param.test_name;
      }
);

struct Itype_Execute_Load_Case {
    OPERATION operation;
    int32_t data_in_mem;
    int32_t expected_rd_value; 
    std::string test_name;
};

class Itype_Execute_Load_Test : public ::testing::TestWithParam<Itype_Execute_Load_Case> {};

TEST_P(Itype_Execute_Load_Test, executes)
{
    auto data_mem = std::make_shared<Memory>();

    Itype_Execute_Load_Case tc = GetParam();

    CPU_test cpu(32, nullptr, data_mem);

    cpu.instruction_fields.type = TYPE::I_TYPE;
    cpu.instruction_fields.Operation = tc.operation;
    cpu.instruction_fields.rs1 = 1;
    cpu.instruction_fields.imm = 0;
    cpu.instruction_fields.rd = 3;

    cpu.writeReg(1, 10);
    data_mem->Write(static_cast<uint32_t>(tc.data_in_mem), 10);

    cpu.Execute();

    auto rd = static_cast<int32_t>(cpu.readReg(3));
    EXPECT_EQ(rd, tc.expected_rd_value);
}

INSTANTIATE_TEST_SUITE_P(I_TYPE_LOAD, Itype_Execute_Load_Test,

    ::testing::Values(
        Itype_Execute_Load_Case{OPERATION::LB, 0xF11, 0x11, "lb_basic"},
        Itype_Execute_Load_Case{OPERATION::LB, static_cast<int32_t>(0xFFFFFFFF), -1, "lb_sign_extends"},

        Itype_Execute_Load_Case{OPERATION::LH, static_cast<int32_t>(0xDEAD0110), 0x0110, "lh_basic"},
        Itype_Execute_Load_Case{OPERATION::LH, static_cast<int32_t>(0xFFFFFFFF), -1, "lh_sign_extends"},

        Itype_Execute_Load_Case{OPERATION::LW, static_cast<int32_t>(0xDEAD0110), static_cast<int32_t>(0xDEAD0110), "lw_basic"},
        Itype_Execute_Load_Case{OPERATION::LW, static_cast<int32_t>(0xFFFFFFFF), -1, "lw_sign_extends"},

        Itype_Execute_Load_Case{OPERATION::LBU, 0xF11, 0x11, "lbu_basic"},
        Itype_Execute_Load_Case{OPERATION::LBU, static_cast<int32_t>(0xFFFFFFFF), 0x0FF, "lbu_does_not_sign_ext"},

        Itype_Execute_Load_Case{OPERATION::LHU, static_cast<int32_t>(0xDEAD0110), 0x0110, "lhu_basic"},
        Itype_Execute_Load_Case{OPERATION::LHU, static_cast<int32_t>(0xFFFFFFFF), 0x0FFFF, "lhu_does_not_sign_ext"}

    ),

    [](const ::testing::TestParamInfo<Itype_Execute_Load_Case>& info) {
      return info.param.test_name;
      }
);

struct Stype_Execute_Case {
    OPERATION operation;
    uint32_t base_address {};
    int32_t offset {};
    uint32_t address_exp;
    int32_t rs2 {}; // value in rs2 to store
    std::string test_name;    
};

class Stype_Execute_Test : public ::testing::TestWithParam<Stype_Execute_Case> {};

TEST_P(Stype_Execute_Test, executes)
{
   std::shared_ptr <Memory> data_memory_ptr = std::make_shared <Memory>();
   CPU_test cpu(32, nullptr, data_memory_ptr);
   Stype_Execute_Case tc = GetParam();
   
   cpu.instruction_fields.type = TYPE::S_TYPE;
   cpu.instruction_fields.Operation = tc.operation;
   cpu.instruction_fields.imm = tc.offset;
   cpu.instruction_fields.rs1 = 1;
   cpu.instruction_fields.rs2 = 2;

   cpu.writeReg(2, tc.rs2);
   cpu.writeReg(1, tc.base_address);
   cpu.Execute();

   auto address = tc.address_exp;
   
   if (tc.operation == OPERATION::SW)
    EXPECT_EQ(static_cast<int32_t>(data_memory_ptr->Read_Word(address)), tc.rs2);

   else if (tc.operation == OPERATION::SH)
    EXPECT_EQ(static_cast<int32_t>(data_memory_ptr->Read_halfWord(address)), (tc.rs2 & 0xFFFF));

   else if (tc.operation == OPERATION::SB)
    EXPECT_EQ(static_cast<int32_t>(data_memory_ptr->Read_Byte(address)), (tc.rs2 & 0xFF));
    
}

INSTANTIATE_TEST_SUITE_P(S_TYPE, Stype_Execute_Test,

    ::testing::Values(
        Stype_Execute_Case{OPERATION::SW, 0, 0, 0, 0xABCDEF, "sw_basic"},
        Stype_Execute_Case{OPERATION::SH, 0, 0, 0, 0xABCDEF, "sh_basic"},
        Stype_Execute_Case{OPERATION::SB, 0, 0, 0, 0xABCDEF, "sb_basic"},

        Stype_Execute_Case{OPERATION::SW, 60, 40,100, 0xABCDEF, "sw_basic_2"},
        Stype_Execute_Case{OPERATION::SH, 500, 400,900, 0xABCDEF, "sh_basic_2"},
        Stype_Execute_Case{OPERATION::SB, 1200, 40, 1240, 0xABCDEF, "sb_basic_2"},

        Stype_Execute_Case{OPERATION::SW, 6000, -700,5300, static_cast<int32_t>(0xFFFFFFFF), "sw_negative_offset"},
        Stype_Execute_Case{OPERATION::SH, 500, -400,100, 0xF, "sh_negative_offset"},
        Stype_Execute_Case{OPERATION::SB, 1200, -1, 1199, 0xABCDEF, "sb_negative_offset"},

        Stype_Execute_Case{OPERATION::SW, 0, 0x7FF, 0x7FF, 0xABCDEF, "max_positive_offset"},
        Stype_Execute_Case{OPERATION::SW, 4000, -2048, 1952, 0xABCDEF, "max_negative_offset"}
        
    ),

    [](const ::testing::TestParamInfo<Stype_Execute_Case>& info) {
      return info.param.test_name;
      }
);

struct Utype_Execute_Case {
    OPERATION operation;
    int32_t imm;
    int32_t expected_rd_value; // expected value
    std::string test_name;
    uint32_t PC = 0;
};

class Utype_Execute_Test : public ::testing::TestWithParam<Utype_Execute_Case> {};

TEST_P(Utype_Execute_Test, executes)
{
    Utype_Execute_Case tc = GetParam();

    CPU_test cpu;
    cpu.instruction_fields.type = TYPE::U_TYPE;
    cpu.instruction_fields.Operation = tc.operation;
    cpu.instruction_fields.rd = 1;
    cpu.instruction_fields.imm = tc.imm;
    cpu.writePC(tc.PC);

    cpu.writeReg(1, 0xFFF); 

    cpu.Execute();

    
    auto rd = static_cast<int32_t>(cpu.readReg(1));
    EXPECT_EQ(rd, tc.expected_rd_value);

}

INSTANTIATE_TEST_SUITE_P(U_TYPE, Utype_Execute_Test,

    ::testing::Values(

      Utype_Execute_Case{OPERATION::LUI, 0x8CDEF, static_cast<int32_t>(0x8CDEF000), "lui_basic"},
      Utype_Execute_Case{OPERATION::LUI, 0xFFFFF, static_cast<int32_t>(0xFFFFF000), "lui_max"},

      Utype_Execute_Case{OPERATION::AUIPC, 0xABCDE, static_cast<int32_t>(0xABCDE001), "auipc_basic", 0x001},
      Utype_Execute_Case{OPERATION::AUIPC, 0xFFFFF, static_cast<int32_t>(0xFFFFFFFF), "auipc_max", 0xFFF}
    ),

    [](const ::testing::TestParamInfo<Utype_Execute_Case>& info) {
      return info.param.test_name;
      }
);

struct Btype_Execute_Case {
    OPERATION operation;
    int32_t rs1_value;
    int32_t rs2_value;
    int32_t imm; 
    uint32_t pc; 
    uint32_t pc_expected;
    std::string test_name;
    
};

class Btype_Execute_Test : public ::testing::TestWithParam<Btype_Execute_Case> {};

TEST_P(Btype_Execute_Test, executes)
{
    Btype_Execute_Case tc = GetParam();

    CPU_test cpu;
    cpu.instruction_fields.type = TYPE::B_TYPE;
    cpu.instruction_fields.Operation = tc.operation;
    cpu.instruction_fields.rs1 = 1;
    cpu.instruction_fields.rs2 = 2;
    cpu.instruction_fields.imm = tc.imm;

    cpu.writePC(tc.pc);
    cpu.writeReg(1, tc.rs1_value); 
    cpu.writeReg(2, tc.rs2_value); 

    cpu.Execute();

    EXPECT_EQ(cpu.readPC(), tc.pc_expected) << "Program counter mismatch";
    
}

INSTANTIATE_TEST_SUITE_P(B_TYPE, Btype_Execute_Test,

    ::testing::Values(

        Btype_Execute_Case{OPERATION::BEQ, 5, 5, 8, 0, 8, "beq_taken"},
        Btype_Execute_Case{OPERATION::BEQ, 5, 10, 16, 0, 4, "beq_not_taken"},
        Btype_Execute_Case{OPERATION::BEQ, -5000, -5000, -4, 16, 12, "beq_taken_negative"},
        Btype_Execute_Case{OPERATION::BEQ, -700, -50, -4, 16, 20, "beq_not_taken_negative"},
        
        Btype_Execute_Case{OPERATION::BNE, 5, 7, 16, 0, 16, "BNE_taken"},
        Btype_Execute_Case{OPERATION::BNE, 100, 100, 16, 0, 4, "BNE_not_taken"},
        Btype_Execute_Case{OPERATION::BNE, -5000, -5001, -4, 16, 12, "BNE_taken_negative"},
        Btype_Execute_Case{OPERATION::BNE, -50, -50, -4, 16, 20, "BNE_not_taken_negative"},

        Btype_Execute_Case{OPERATION::BLT, 5, 7, 16, 0, 16, "BLT_taken"},
        Btype_Execute_Case{OPERATION::BLT, 100, 100, 16, 0, 4, "BLT_not_taken"},
        Btype_Execute_Case{OPERATION::BLT, 1000, 100, 16, 0, 4, "BLT_not_taken_2"},
        Btype_Execute_Case{OPERATION::BLT, -20000, -19000, -400, 400, 0, "BLT_taken_negative"},
        Btype_Execute_Case{OPERATION::BLT, -20000, -20000, -400, 400, 404, "BLT_not_taken_negative"},
        Btype_Execute_Case{OPERATION::BLT, -20000, -22000, -400, 400, 404, "BLT_not_taken_negative_2"},
        Btype_Execute_Case{OPERATION::BLT, 0x7FFFFFFF, static_cast<int32_t>(0xFFFFFFFF), 200, 1000, 1004, "blt_is_signed"},

        Btype_Execute_Case{OPERATION::BGE, 5, -7, 16, 0, 16, "BGE_taken"},
        Btype_Execute_Case{OPERATION::BGE, 100, 100, 16, 0, 16, "BGE_taken_2"},
        Btype_Execute_Case{OPERATION::BGE, 99, 100, 16, 0, 4, "BGE_not_taken"},
        Btype_Execute_Case{OPERATION::BGE, -20000, -19000, -400, 400, 404, "BGE__not_taken_negative"},
        Btype_Execute_Case{OPERATION::BGE, -20000, -20000, -400, 400, 0, "BGE_taken_negative"},
        Btype_Execute_Case{OPERATION::BGE, -20000, -22000, -400, 400, 0, "BGE_taken_negative_2"},
        Btype_Execute_Case{OPERATION::BGE, 0x7FFFFFFF, static_cast<int32_t>(0xFFFFFFFF), -200, 1000, 800, "bge_is_signed"},

        Btype_Execute_Case{OPERATION::BLTU, 5, 7, 16, 0, 16, "BLTU_taken"},
        Btype_Execute_Case{OPERATION::BLTU, 100, 100, 16, 0, 4, "BLTU_not_taken"},
        Btype_Execute_Case{OPERATION::BLTU, 1000, 100, 16, 0, 4, "BLTU_not_taken_2"},
        Btype_Execute_Case{OPERATION::BLTU, 0x7FFFFFFF, static_cast<int32_t>(0xFFFFFFFF), 200, 1000, 1200, "bltu_is_unsigned"},

        Btype_Execute_Case{OPERATION::BGEU, 7000, 7, 16, 0, 16, "BGEU_taken"},
        Btype_Execute_Case{OPERATION::BGEU, 100, 100, 16, 0, 16, "BGEU_taken_2"},
        Btype_Execute_Case{OPERATION::BGEU, 99, 100, 16, 0, 4, "BGEU_not_taken"},
        Btype_Execute_Case{OPERATION::BGEU, 0x7FFFFFFF, static_cast<int32_t>(0xFFFFFFFF), -200, 8, 12, "bgeu_is_unsigned"}
    ),
    
    [](const ::testing::TestParamInfo<Btype_Execute_Case>& info) {
      return info.param.test_name;
      }
);

TEST(Jtype_execute_test, test_JAL)
{
    int32_t jump_target_address{200};

    CPU_test cpu;
    cpu.instruction_fields.type = TYPE::J_TYPE;
    cpu.instruction_fields.Operation = OPERATION::JAL;
    cpu.instruction_fields.rd = 1;
    cpu.instruction_fields.imm =  jump_target_address;

    cpu.writePC(400);
    cpu.Execute();

    EXPECT_EQ(cpu.readReg(1), 404);
    EXPECT_EQ(cpu.readPC(), 600);

}

TEST(Jtype_execute_test, test_JAL_2)
{
    int32_t jump_target_address{0xA67F8};

    CPU_test cpu;
    cpu.instruction_fields.type = TYPE::J_TYPE;
    cpu.instruction_fields.Operation = OPERATION::JAL;
    cpu.instruction_fields.rd = 1;
    cpu.instruction_fields.imm =  jump_target_address;

    cpu.writePC(0x0000540C);
    cpu.Execute();

    EXPECT_EQ(cpu.readReg(1), 0x00005410);
    EXPECT_EQ(cpu.readPC(), 0x000ABC04);

}

TEST(Jtype_execute_test, test_JAL_max)
{
    int32_t jump_target_address{1048574};

    CPU_test cpu;
    cpu.instruction_fields.type = TYPE::J_TYPE;
    cpu.instruction_fields.Operation = OPERATION::JAL;
    cpu.instruction_fields.rd = 1;
    cpu.instruction_fields.imm =  jump_target_address;

    cpu.writePC(400);
    cpu.Execute();

    EXPECT_EQ(cpu.readReg(1), 404);
    EXPECT_EQ(cpu.readPC(), 1048974);

}

TEST(Jtype_execute_test, test_JAL_negative_max)
{
    int32_t jump_target_address{-1048576};

    CPU_test cpu;
    cpu.instruction_fields.type = TYPE::J_TYPE;
    cpu.instruction_fields.Operation = OPERATION::JAL;
    cpu.instruction_fields.rd = 1;
    cpu.instruction_fields.imm =  jump_target_address;

    cpu.writePC(0xFFFFFFFA);
    cpu.Execute();

    EXPECT_EQ(cpu.readReg(1), 0xFFFFFFFE);
    EXPECT_EQ(cpu.readPC(), static_cast<uint32_t>(4293918714));

}