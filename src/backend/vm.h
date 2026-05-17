#pragma once
#include <vector>
#include <cstdint>
#include "../core/opcodes.h"

class VM
{
public:
    // Takes the compiled bytecode array and runs it
    void execute(const std::vector<uint8_t> &bytecode);

private:
    std::vector<int> stack;
    int ip = 0; // Instruction Pointer

    // A simple array to act as RAM for our variables.
    // Since our var IDs are uint8_t, there can only be 256 variables max right now.
    int memory[256] = {0};

    // Fast stack helpers
    void push(int value);
    int pop();
};