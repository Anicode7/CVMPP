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

uint16_t read16(const std::vector<uint8_t> &bytecode, size_t &ip)
{
    uint16_t value = (bytecode[ip] << 8) | bytecode[ip + 1];
    ip += 2;
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
            // Now reads 2 bytes for the number
            uint16_t value = read16(bytecode, ip);
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
            // Now reads a 2-byte memory ID
            uint16_t varId = read16(bytecode, ip);
            int value = pop();
            memory[varId] = value;
            break;
        }
        case OpCode::LOAD_VAR:
        {
            // Now reads a 2-byte memory ID
            uint16_t varId = read16(bytecode, ip);
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
            uint16_t target = read16(bytecode, ip); // Read 2-byte target
            int condition = pop();
            if (condition == 0)
            {
                ip = target;
            }
            break;
        }
        case OpCode::JUMP:
        {
            uint16_t target = read16(bytecode, ip); // Read 2-byte target
            ip = target;
            break;
        }
        case OpCode::INPUT:
        {
            int userInput;
            std::cout << "? "; // A simple prompt character for the user
            std::cin >> userInput;

            // Clear the input buffer so the REPL doesn't glitch out afterwards
            std::cin.ignore(256, '\n');

            push(userInput);
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