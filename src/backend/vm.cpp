#include "vm.h"
#include <iostream>
#include <stdexcept>

void VM::push(int value)
{
    stack.push_back(value);
}

int VM::pop()
{
    if (stack.empty())
    {
        throw std::runtime_error("VM Error: Stack Underflow (tried to pop an empty stack)");
    }
    int value = stack.back();
    stack.pop_back();
    return value;
}

void VM::execute(const std::vector<uint8_t> &bytecode)
{
    ip = 0;

    while (ip < bytecode.size())
    {
        // Fetch the current instruction and advance the pointer
        uint8_t instruction = bytecode[ip++];
        OpCode opcode = static_cast<OpCode>(instruction);

        switch (opcode)
        {
        case OpCode::PUSH:
        {
            // The next byte is the actual number
            uint8_t value = bytecode[ip++];
            push(value);
            break;
        }
        case OpCode::ADD:
        {
            int right = pop();
            int left = pop();
            push(left + right);
            break;
        }
        case OpCode::SUB:
        {
            int right = pop();
            int left = pop();
            push(left - right);
            break;
        }
        case OpCode::MUL:
        {
            int right = pop();
            int left = pop();
            push(left * right);
            break;
        }
        case OpCode::DIV:
        {
            int right = pop();
            int left = pop();
            if (right == 0)
                throw std::runtime_error("VM Error: Division by zero");
            push(left / right);
            break;
        }
        case OpCode::STORE_VAR:
        {
            // The next byte is the variable's memory ID
            uint8_t varId = bytecode[ip++];
            int value = pop();
            memory[varId] = value;
            break;
        }
        case OpCode::LOAD_VAR:
        {
            uint8_t varId = bytecode[ip++];
            push(memory[varId]);
            break;
        }
        case OpCode::PRINT:
        {
            int value = pop();
            std::cout << value << "\n";
            break;
        }
        case OpCode::EQUAL:
        {
            int right = pop();
            int left = pop();
            push(left == right ? 1 : 0);
            break;
        }
        case OpCode::LESS:
        {
            int right = pop();
            int left = pop();
            push(left < right ? 1 : 0);
            break;
        }
        case OpCode::JUMP_IF_FALSE:
        {
            uint8_t target = bytecode[ip++]; // Read the destination index
            int condition = pop();
            if (condition == 0)
            {
                ip = target; // TIME TRAVEL!
            }
            break;
        }
        case OpCode::JUMP:
        {
            uint8_t target = bytecode[ip++];
            ip = target; // TIME TRAVEL!
            break;
        }
        case OpCode::HALT:
        {
            return; // Stop the execution loop gracefully
        }
        default:
        {
            throw std::runtime_error("VM Error: Unknown OpCode encountered.");
        }
        }
    }
}