# RISC-V simulator

A cycle-accurate RISC-V instruction set simulator currently supporting the RISC-V base integer instruction set (RV32I) and the M-extension (multiply/divide).

---

## Usage

```bash
build/rv32_sim program.bin -p=true
```
The simulator reads an assembled RISC-V binary executable, the optional command `-p=true` prints the architectural state of the CPU (Registers + PC) after execution. The CPU will run until it encounters the `ebreak` instruction or reaches the maximum number of cycles (default = 5000) for safety.

To make assembled binary programs check out my [RISC-V assembler](https://github.com/fyz-m/RV32I-Assembler).  

### Example

In `tests/programs` there is an assembled binary that calculates the factorial of 12 (maximum factorial that can fit in 32-bits!), to run it:
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
| `test_decode.cpp` | Instruction fields decoding and immediate extraction for all six encoding formats |
| `test_execute.cpp` | ALU execution, load/store, branches, jumps |
| `test_memory.cpp` | Register file, byte-addressable memory, bounds checking |
 
Edge cases tested:
- **Immediate sign extension and maximum size**   
- **Arithmetic overflow** 
- **Shift correctness** 
- **M-extension corner cases**
- **Unsigned comparisons** 

To run the test suite: 
```bash
./build/iss_tests 
```
**WARNING:** the memory test suite contains a test for the maximum memory size possilbe in RV32. This will attempt to allocate ~4GB on the heap which could potentially crash your computer. Add this flag to skip this test:
```bash
--gtest_filter=-MemoryTest.max_resize
```
---

## Build
 
**Prerequisites:** CMake ≥ 3.28, g++ 13.3 (c++20).
 
```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```


