#pragma once
#include <vector>
#include <cstdint>
#include "../core/opcodes.h"
#include "../core/value.h" // <-- NEW: Include our Tagged Union

class VM
{
public:
    void execute(const std::vector<uint8_t> &bytecode);

    // Upgraded memory array to hold dynamic values
    std::vector<Value> memory;

    VM()
    {
        // Initialize memory with integer 0s to prevent undefined behavior
        memory.resize(65536, Value::createInt(0));
    }

private:
    std::vector<Value> stack; // Upgraded stack
    size_t ip = 0;

    void push(Value value);
    Value pop();
    uint16_t read16(const std::vector<uint8_t> &bytecode, size_t &ip);
};