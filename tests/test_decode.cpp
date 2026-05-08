#include "gtest/gtest.h"
#include <cstdint>
#include <sys/types.h>
#include "../src/headers/decode.hpp"


struct Rtype_Decode_Case {
    uint32_t instruction;
    uint8_t expected_rd, expected_rs1, expected_rs2;
    OPERATION expected_op; 
    std::string test_name;
};

class RtypeTest : public ::testing::TestWithParam<Rtype_Decode_Case> {};

TEST_P(RtypeTest, decode_R_type_instructions)
{
    Rtype_Decode_Case tc = GetParam();

    DecodedInstruction fields;
    fields.raw_inst = tc.instruction;
    
    decode(fields);

    EXPECT_EQ(fields.rd, tc.expected_rd);
    EXPECT_EQ(fields.rs1, tc.expected_rs1);
    EXPECT_EQ(fields.rs2, tc.expected_rs2);
    EXPECT_EQ(fields.Operation, tc.expected_op);
}

INSTANTIATE_TEST_SUITE_P(R_TYPE, RtypeTest,

    ::testing::Values(
      Rtype_Decode_Case {0x003100b3, 1, 2, 3, OPERATION::ADD, "add_basic"},  // add x1, x2, x3
      Rtype_Decode_Case {0x403100b3, 1, 2, 3 , OPERATION::SUB, "sub_basic"}, // sub x1, x2, x3

      Rtype_Decode_Case {0x02000033, 0, 0, 0, OPERATION::MUL, "MUL_basic"},  // mul x0, x0, x0
      Rtype_Decode_Case {0x02001033, 0, 0, 0 , OPERATION::MULH, "MULH_basic"}, // mulh x0, x0, x0

      Rtype_Decode_Case {0x02002033, 0, 0, 0, OPERATION::MULHSU, "MULHSU_basic"},  // mulhsu x0, x0, x0
      Rtype_Decode_Case {0x02003033, 0, 0, 0 , OPERATION::MULHU, "MULHU_basic"}, // mulhu x0, x0, x0

      Rtype_Decode_Case {0x02004033, 0, 0, 0, OPERATION::DIV, "DIV_basic"},  // div x0, x0, x0
      Rtype_Decode_Case {0x02005033, 0, 0, 0 , OPERATION::DIVU, "DIVU_basic"}, // divu x0, x0, x0

      Rtype_Decode_Case {0x02006033, 0, 0, 0, OPERATION::REM, "REM_basic"},  // rem x0, x0, x0
      Rtype_Decode_Case {0x02007033, 0, 0, 0 , OPERATION::REMU, "REMU_basic"} // remu x0, x0, x0

    ),

    [](const ::testing::TestParamInfo<Rtype_Decode_Case>& info) {
      return info.param.test_name;
      }
);


struct Itype_Decode_Case {
    uint32_t instruction;
    uint8_t expected_rd, expected_rs1; 
    int32_t expected_imm;
    OPERATION expected_op;
    std::string test_name; 
};

class ItypeTest : public ::testing::TestWithParam<Itype_Decode_Case> {};

TEST_P(ItypeTest, decode_I_type_instructions)
{
    Itype_Decode_Case tc = GetParam();

    DecodedInstruction fields;
    fields.raw_inst = tc.instruction;

    decode(fields);

    EXPECT_EQ(fields.rd, tc.expected_rd);
    EXPECT_EQ(fields.rs1, tc.expected_rs1);
    EXPECT_EQ(fields.imm, tc.expected_imm) << "Incorrect immediate extraction (may not be properly sign extended)";
    EXPECT_EQ(fields.Operation, tc.expected_op);
}

INSTANTIATE_TEST_SUITE_P(I_TYPE, ItypeTest,

    ::testing::Values(
      Itype_Decode_Case {0x02810093, 1, 2, 40, OPERATION::ADDI, "addi_basic"},  // addi x1, x2, 40
      Itype_Decode_Case {0x00028503, 10, 5, 0 , OPERATION::LB, "lb_basic"}, // lb x10, 0(x5)
      Itype_Decode_Case{0x001f1f93, 31, 30, 1, OPERATION::SLLI, "slli_basic"}, // slli x31, x30, 1
      Itype_Decode_Case{0x01409093, 1, 1, 20, OPERATION::SLLI, "slli_basic_2"}, // slli x1, x1, 20
      Itype_Decode_Case{0xff605003, 0, 0, -10, OPERATION::LHU, "lhu_basic"}, // lhu x0, -10(x0)
      // Corner case: both srli and srai have the same opcode and funct3
      Itype_Decode_Case{0x00abdc93, 25, 23, 10, OPERATION::SRLI, "srli_basic"}, // srli x25, x23, 10  
      Itype_Decode_Case{0x4081d613, 12, 3, 8, OPERATION::SRAI, "srai_basic"}, // srai x12, x3, 8 
      Itype_Decode_Case{0x4020d093, 1, 1, 2, OPERATION::SRAI, "srai_basic_2"}, // srai x1, x1, 2  
      // Test immediate
      Itype_Decode_Case {0xfec10093, 1, 2, -20, OPERATION::ADDI, "sign_ext"},  // addi x1, x2, -20
      Itype_Decode_Case {0xfff10093, 1, 2, -1, OPERATION::ADDI, "sign_ext_2"}, // addi x1, x2, -1
      Itype_Decode_Case{0x7ff08093, 1, 1, 2047, OPERATION::ADDI, "I_type_max_imm"}, // addi x1, x1, 2047
      Itype_Decode_Case{0x80008093, 1, 1, -2048, OPERATION::ADDI, "I_type_min_imm"} // addi x1, x1, -2048
    ),

    [](const ::testing::TestParamInfo<Itype_Decode_Case>& info) {
      return info.param.test_name;
      }
);

struct Stype_Decode_Case {
    uint32_t instruction;
    uint8_t expected_rs2; 
    int32_t expected_imm;
    uint8_t expected_rs1;
    OPERATION expected_op;
    std::string test_name; 
};

class StypeTest : public ::testing::TestWithParam<Stype_Decode_Case> {};

TEST_P(StypeTest, decode_S_type_instructions)
{
    Stype_Decode_Case tc = GetParam();

    DecodedInstruction fields;
    fields.raw_inst = tc.instruction;

    decode(fields);

    EXPECT_EQ(fields.rs1, tc.expected_rs1);
    EXPECT_EQ(fields.rs2, tc.expected_rs2);
    EXPECT_EQ(fields.imm, tc.expected_imm) << "Incorrect immediate extraction (may not be properly sign extended)";
    EXPECT_EQ(fields.Operation, tc.expected_op);
}

INSTANTIATE_TEST_SUITE_P(S_TYPE, StypeTest,

    ::testing::Values(
      Stype_Decode_Case {0x19488823, 20, 400, 17, OPERATION::SB, "sb_basic" }, // sb x20, 400(x17)
      Stype_Decode_Case {0x00a01023, 10, 0, 0, OPERATION::SH, "sh_basic" }, // sh x10, 0(x0)
      Stype_Decode_Case {0x7e09a623, 0, 2028, 19, OPERATION::SW, "sw_basic" }, // sw x0, 2028(x19)

      Stype_Decode_Case {0x80208023, 2, -2048, 1, OPERATION::SB, "sb_negative_imm" },  // sb x2, -2048(x1)
      Stype_Decode_Case {0xfdf09723, 31, -50, 1, OPERATION::SH, "sh_negative_imm" },  // sh x31, -50(x1)
      Stype_Decode_Case {0xfef6afa3, 15, -1, 13, OPERATION::SW, "sw_negative_imm" },   // sw x15, -1(x13)

      Stype_Decode_Case{0x7e10afa3, 1, 2047, 1, OPERATION::SW, "Stype_max_imm"}, // sw x1, 2047(x1)
      Stype_Decode_Case{0x8010a023, 1, -2048, 1, OPERATION::SW, "Stype_min_imm"} // sw x1, -2048(x1) 
 
    ),

    [](const ::testing::TestParamInfo<Stype_Decode_Case>& info) {
      return info.param.test_name;
      }
);

struct Btype_Decode_Case {
    uint32_t instruction;
    uint8_t expected_rs1; 
    uint8_t expected_rs2;
    int32_t expected_imm;
    OPERATION expected_op;
    std::string test_name; 
};

class BtypeTest : public ::testing::TestWithParam<Btype_Decode_Case> {};

TEST_P(BtypeTest, decode_B_type_instructions)
{
    Btype_Decode_Case tc = GetParam();

    DecodedInstruction fields;
    fields.raw_inst = tc.instruction;

    decode(fields);

    EXPECT_EQ(fields.rs1, tc.expected_rs1);
    EXPECT_EQ(fields.rs2, tc.expected_rs2);
    EXPECT_EQ(fields.imm, tc.expected_imm) << "Incorrect immediate extraction (may not be properly sign extended)";
    EXPECT_EQ(fields.Operation, tc.expected_op);
}

INSTANTIATE_TEST_SUITE_P(B_TYPE, BtypeTest,

    ::testing::Values(
      Btype_Decode_Case{0x014000e3, 0, 20, 2048, OPERATION::BEQ, "beq_basic"}, // beq x0, x20, 2048
      Btype_Decode_Case{0x00109663, 1,1, 12, OPERATION::BNE, "bne_basic"}, // bne x1, x1, 12
      Btype_Decode_Case{0x0010c663, 1,1, 12, OPERATION::BLT, "blt_basic"}, // blt x1, x1, 12
      Btype_Decode_Case{0x0010d663, 1,1, 12, OPERATION::BGE, "bge_basic"}, // bge x1, x1, 12
      Btype_Decode_Case{0x0010e663, 1,1, 12, OPERATION::BLTU, "bltu_basic"}, // bltu x1, x1, 12
      Btype_Decode_Case{0x0010f663, 1,1, 12, OPERATION::BGEU, "bgeu_basic"}, // bgeu x1, x1, 12
      
      // Immediate tests 
      Btype_Decode_Case{0xfe108ae3, 1,1, -12, OPERATION::BEQ, "imm_negative"}, // bge x1, x1, -12 
      Btype_Decode_Case{0x00108263, 1, 1, 4, OPERATION::BEQ, "imm_min_positive"}, // beq x1, x1, 4
      Btype_Decode_Case{0xfe108ee3, 1, 1, -4, OPERATION::BEQ, "imm_min_negative"}, // beq x1, x1, -4
      Btype_Decode_Case{0x81400063, 0, 20, -4096, OPERATION::BEQ, "imm_min"}, // beq x0, x20, -4096
      Btype_Decode_Case{0x7f400fe3, 0, 20, 4094, OPERATION::BEQ, "imm_max"} // beq x0, x20, 4094
      
    ),

    [](const ::testing::TestParamInfo<Btype_Decode_Case>& info) {
      return info.param.test_name;
      }
);

struct Utype_Decode_Case {
    uint32_t instruction;
    uint8_t expected_rd; 
    int32_t expected_imm;
    OPERATION expected_op;
    std::string test_name; 
};

class UtypeTest : public ::testing::TestWithParam<Utype_Decode_Case> {};

TEST_P(UtypeTest, decode_U_type_instructions)
{
    Utype_Decode_Case tc = GetParam();

    DecodedInstruction fields;
    fields.raw_inst = tc.instruction;

    decode(fields);

    EXPECT_EQ(fields.rd, tc.expected_rd);
    EXPECT_EQ(fields.imm, tc.expected_imm) << "Incorrect immediate extraction (may not be properly sign extended)";
    EXPECT_EQ(fields.Operation, tc.expected_op);
}

INSTANTIATE_TEST_SUITE_P(U_TYPE, UtypeTest,

    ::testing::Values(
      Utype_Decode_Case{0x000142b7, 5, 20, OPERATION::LUI, "lui_basic"}, // lui x5, 20
      Utype_Decode_Case{0x00014297, 5, 20, OPERATION::AUIPC, "auipc_basic"}, // auipc x5, 20

      // Immediate tests
      Utype_Decode_Case{0xffe0c0b7, 1, -500,OPERATION::LUI, "u_type_imm_negative"}, // lui, x1, -500
      Utype_Decode_Case{0x800000b7, 1, -524288,OPERATION::LUI, "u_type_imm_min"}, // lui, x1, -524288
      Utype_Decode_Case{0x7ffff0b7, 1, 524287,OPERATION::LUI, "u_type_imm_max"} // lui, x1, 524287

    ),

    [](const ::testing::TestParamInfo<Utype_Decode_Case>& info) {
      return info.param.test_name;
      }
);


struct Jtype_Decode_Case {
    uint32_t instruction;
    uint8_t expected_rd; 
    int32_t expected_imm;
    OPERATION expected_op;
    std::string test_name; 
};

class JtypeTest : public ::testing::TestWithParam<Jtype_Decode_Case> {};

TEST_P(JtypeTest, decode_J_type_instructions)
{
    Jtype_Decode_Case tc = GetParam();

    DecodedInstruction fields;
    fields.raw_inst = tc.instruction;

    decode(fields);

    EXPECT_EQ(fields.rd, tc.expected_rd);
    EXPECT_EQ(fields.imm, tc.expected_imm) << "Incorrect immediate extraction (may not be properly sign extended)";
    EXPECT_EQ(fields.Operation, tc.expected_op);
}

INSTANTIATE_TEST_SUITE_P(J_TYPE, JtypeTest,

    ::testing::Values(
        Jtype_Decode_Case{0x0c800a6f, 20, 200, OPERATION::JAL, "jal_basic"}, // jal x20, 200

        // Immediate tests
        Jtype_Decode_Case{0xf39ffa6f, 20, -200, OPERATION::JAL, "jal_imm_negative"}, // jal x20, -200
        Jtype_Decode_Case{0x80000a6f, 20, -1048576, OPERATION::JAL, "jal_imm_negative_max"}, // jal x20, -1048576
        Jtype_Decode_Case{0xffdffa6f, 20, -4, OPERATION::JAL, "jal_imm_negative_min"}, // jal x20, -4
        Jtype_Decode_Case{0x00400a6f, 20, 4, OPERATION::JAL, "jal_imm_positive_min"}, // jal x20, 4
        Jtype_Decode_Case{0x7ffffa6f, 20, 1048574, OPERATION::JAL, "jal_imm_positive_max"} // jal x20, 1048574
    ),

    [](const ::testing::TestParamInfo<Jtype_Decode_Case>& info) {
      return info.param.test_name;
      }
);



  
