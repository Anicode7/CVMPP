#include "vm.h"
#include <iostream>
#include <stdexcept>

void VM::push(Value value)
{
    stack.push_back(value);
}

Value VM::pop()
{
    if (stack.empty())
    {
        throw std::runtime_error("Stack underflow!");
    }
    Value value = stack.back();
    stack.pop_back();
    return value;
}

uint16_t VM::read16(const std::vector<uint8_t> &bytecode, size_t &ip)
{
    uint16_t high = bytecode[ip++];
    uint16_t low = bytecode[ip++];
    return (high << 8) | low;
}

void VM::execute(const std::vector<uint8_t> &bytecode)
{
    ip = 0;
    while (ip < bytecode.size())
    {
        OpCode instruction = static_cast<OpCode>(bytecode[ip++]);

        switch (instruction)
        {
        case OpCode::PUSH:
        {
            uint16_t value = read16(bytecode, ip);
            push(Value::createInt(value)); // Wrap the raw number in our Tagged Union
            break;
        }
        case OpCode::ADD:
        case OpCode::SUB:
        case OpCode::MUL:
        case OpCode::DIV:
        {
            Value b = pop();
            Value a = pop();

            // Strict Type Checking!
            if (!a.isInt() || !b.isInt())
            {
                throw std::runtime_error("Math operations require numbers.");
            }

            int right = b.as.number;
            int left = a.as.number;

            if (instruction == OpCode::ADD)
                push(Value::createInt(left + right));
            else if (instruction == OpCode::SUB)
                push(Value::createInt(left - right));
            else if (instruction == OpCode::MUL)
                push(Value::createInt(left * right));
            else if (instruction == OpCode::DIV)
            {
                if (right == 0)
                    throw std::runtime_error("Division by zero!");
                push(Value::createInt(left / right));
            }
            break;
        }
        case OpCode::NEGATE:
        {
            Value v = pop();
            if (!v.isInt())
                throw std::runtime_error("Cannot negate a non-number.");
            push(Value::createInt(-v.as.number));
            break;
        }
        case OpCode::STORE_VAR:
        {
            uint16_t id = read16(bytecode, ip);
            memory[id] = pop();
            break;
        }
        case OpCode::LOAD_VAR:
        {
            uint16_t id = read16(bytecode, ip);
            push(memory[id]);
            break;
        }
        case OpCode::PRINT:
        {
            Value v = pop();
            if (v.isInt())
                std::cout << v.as.number << "\n";
            else if (v.isBool())
                std::cout << (v.as.boolean ? "true" : "false") << "\n";
            else if (v.isString())
                std::cout << v.stringObj << "\n";
            break;
        }
        case OpCode::EQUAL:
        {
            Value b = pop();
            Value a = pop();
            if (a.type != b.type)
            {
                push(Value::createBool(false));
            }
            else if (a.isInt())
            {
                push(Value::createBool(a.as.number == b.as.number));
            }
            else if (a.isBool())
            {
                push(Value::createBool(a.as.boolean == b.as.boolean));
            }
            else
            {
                push(Value::createBool(a.stringObj == b.stringObj));
            }
            break;
        }
        case OpCode::LESS:
        {
            Value b = pop();
            Value a = pop();
            if (!a.isInt() || !b.isInt())
                throw std::runtime_error("Can only compare numbers with '<'.");
            push(Value::createBool(a.as.number < b.as.number));
            break;
        }
        case OpCode::JUMP_IF_FALSE:
        {
            uint16_t target = read16(bytecode, ip);
            Value condition = pop();

            // Allow numbers to act as booleans (0 is false, everything else is true)
            bool isTruthy = false;
            if (condition.isBool())
                isTruthy = condition.as.boolean;
            else if (condition.isInt())
                isTruthy = (condition.as.number != 0);

            if (!isTruthy)
                ip = target;
            break;
        }
        case OpCode::JUMP:
        {
            ip = read16(bytecode, ip);
            break;
        }
        case OpCode::INPUT:
        {
            std::cout << "? ";
            int val;
            std::cin >> val;
            push(Value::createInt(val));
            break;
        }
        case OpCode::HALT:
            return;
        }
    }
}