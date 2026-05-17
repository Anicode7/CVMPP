#pragma once
#include <cstdint>

// These are the raw byte instructions our VM will execute
enum class OpCode : uint8_t
{
    PUSH,          // Push a number onto the stack
    ADD,           // Pop 2 numbers, Add, Push result
    SUB,           // Pop 2 numbers, Subtract, Push result
    MUL,           // Pop 2 numbers, Multiply, Push result
    DIV,           // Pop 2 numbers, Divide, Push result
    STORE_VAR,     // Pop a number, store it in memory at a specific ID
    LOAD_VAR,      // Look up a number by ID, Push it to stack
    PRINT,         // Pop a number and print it
    HALT,          // Stop execution
    EQUAL,         // : Pop 2, compare (==), push 1 or 0
    LESS,          // : Pop 2, compare (<), push 1 or 0
    JUMP_IF_FALSE, // Pops a boolean. If 0 (false), jumps IP to the target index
    JUMP,
    INPUT
};