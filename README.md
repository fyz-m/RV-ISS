# RISC-V simulator

A RISC-V instruction set simulator currently supporting the RISC-V base integer instruction set (RV32I) and the M-extension (multiply/divide).

## What This Is
 
The simulator executes RV32IM machine code binaries, modelling the fetch/decode/execute pipeline of a single-cycle 32-bit RISC-V core. It implements all RV32I base instructions and all 8 M-extension instructions, with correct handling of spec-defined corner cases such as division by zero, division overflow, and signed/unsigned remainder behaviour.
  
## Features
- Full RV32I base ISA (R, I, S, B, U, J-type instructions)
- M-extension: `MUL`, `MULH`, `MULHU`, `MULHSU`, `DIV`, `DIVU`, `REM`, `REMU`
- Harvard memory model with separate instruction and data address spaces
- Little-endian Byte-addressable memory
- EBREAK halt detection
- `runtoEbreak()` execution mode and single-step `Step()` interface
- Google Test suite with 200+ parameterised tests across all instruction types including edge-cases

---

## Architecture
<div align="center">
<img width="800" height="500" alt="image" src="https://github.com/user-attachments/assets/cb7b76cb-be95-461d-b7d0-62b29c8e228c" />
</div>

### Fetch
A CPU cycle starts by fetching the instruction. The `Fetch()` function reads the instruction from the `instruction memory` and writes it to the `instruction register`. The memory address of the instruction to fetch is held in the `program counter`.
### Decode
Next, the 32-bit instruction is decoded by `Decode()` in order to extract the instruction fields. Firstly, the `opcode` field is found which determines the instruction type (e.g `R-type`) that tells us where the rest of the instruction fields are located in the 32-bit instruction. All RISC-V instructions contain the opcode in bits[6:0] of the instruction. 
<div align="center">
<img width="800" height="450" alt="image" src="https://github.com/user-attachments/assets/0baf6711-9209-4ecd-a540-f932f1531219" />
</div>
<div align="center">
  Source: <a href="https://pages.hmc.edu/harris/ddca/ddcarv.html">Digital Design and Computer Architecture RISC-V Edition</a>
</div>  
<br>

After determining the instruction type, the rest of the fields (e.g `rs1`, `rs2`, `rd`) are extracted and stored in a `DecodedInstruction` struct defined in `cpu.hpp`. The combination of `opcode`, `funct3` and `funct7` fields determines the operation to perform such as `add`.

### Execute
The operation is performed on the operands defined in the instruction. In the case of `add`, the first operand is in the first source register `rs1` and the second is in `rs2`. The 32-bit operands are read from the register file and added together, the result gets written to register `rd` and the program counter is incremented to the next instruction. It can be thought of as an instruction pointer because it contains the memory address of the instruction to execute. 

Some instructions read/write to the data memory (RAM) or change the value of the program counter conditionally such as branch instructions which are `B-type` or unconditionally like `J-type` instructions.       


## Project Structure
```
rv32-iss/
├── CMakeLists.txt
├── README.md
├── src/
│   ├── main.cpp     # CLI entry point 
│   ├── cpu.cpp      # CPU class    
│   ├── decode.cpp   # Instruction decoding logic
│   ├── execute.cpp  # Instruction execution logic
│   ├── memory.cpp   # Byte-addressable memory and register file classes 
│   └── headers/
│       ├── cpu.hpp
│       ├── decode.hpp
│       └── memory.hpp
└── tests/
    ├── test_decode.cpp  # Unit tests for decoding logic
    ├── test_execute.cpp # Unit tests for execution logic
    ├── test_memory.cpp  # Unit tests for memory/register file
    └── programs/
        └── factorial_12.bin # RISC-V program that caclulates 12!  
```
## Build
 
**Prerequisites:** CMake ≥ 3.28, g++ 13.3 (c++20).
 
```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

## Usage

```bash
build/rv32_sim program.bin -p=true
```
The simulator reads an assembled RISC-V binary executable, the optional command `-p=true` prints the architectural state of the CPU (Registers + PC) after execution. The CPU will run until it encounters the `ebreak` instruction or reaches the maximum number of cycles (default = 5000) for safety.

To make assembled binary programs check out my [RISC-V assembler](https://github.com/fyz-m/RV32I-Assembler).  

### Example

In `tests/programs` there is an assembled binary that calculates the factorial of 12 (maximum factorial that can fit in 32-bits) to run it:
```bash
build/rv32_sim tests/programs/factorial_12.bin -p=true
```
Which outputs the following:
```
Calculated 12! in 143 cycles
Register a0 = 479001600

---------------------------- ARCHITECTURAL STATE ----------------------------

Program Counter: 72
Instruction Register : 1048691

Register 0 : 0
Register 1 : 72
Register 2 : 1000
...
...
Register 31 : 0
```
 
## Tests

The CPU was unit tested using GoogleTest. 
 
| File | What it tests |
|---|---|
| `test_decode.cpp` | Instruction fields decoding and immediate extraction |
| `test_execute.cpp` | ALU execution, load/store, branches, jumps |
| `test_memory.cpp` | Register file, byte-addressable memory, bounds checking |
 
Edge cases tested:
- Immediate sign extension and maximum size   
- Arithmetic overflow 
- Shift correctness 
- M-extension corner cases
- Unsigned comparisons

To run the test suite: 
```bash
./build/iss_tests 
```
**WARNING:** the memory test suite contains a test for the maximum memory size possilbe in RV32. This will attempt to allocate ~4GB on the heap which could potentially crash your computer. Add this flag to skip this test:
```bash
--gtest_filter=-MemoryTest.max_resize
```



## Future additions
 - **Add `RVF` intsructions (single-precision floating-point)**  
 - **Add Cache**


