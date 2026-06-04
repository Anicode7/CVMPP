#include "vm.h"
#include <iostream>
#include <stdexcept>

void VM::push(Value value) { stack.push_back(value); }

Value VM::pop()
{
    if (stack.empty())
        throw std::runtime_error("Stack underflow!");
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

void VM::execute(const std::vector<uint8_t> &mainBytecode, const std::vector<std::shared_ptr<FunctionObj>> &compilerFuncs)
{
    // FIX: Just perfectly sync the VM's memory to the Compiler's master list!
    functions = compilerFuncs;

    auto mainFunc = std::make_shared<FunctionObj>("main", 0);
    mainFunc->chunk = mainBytecode;
    frames.clear();
    frames.push_back(CallFrame(mainFunc));

    // Keep the Scope Amnesia fix!
    while (scopes.size() > 1)
    {
        scopes.pop_back();
    }
    if (scopes.empty())
    {
        scopes.push_back({});
    }
    // ... rest of the function stays exactly the same
    while (!frames.empty())
    {
        // ... KEEP EVERYTHING BELOW THIS EXACTLY THE SAME!
        CallFrame &frame = frames.back();

        if (frame.ip >= frame.function->chunk.size())
        {
            frames.pop_back();
            if (scopes.size() > 1)
                scopes.pop_back();
            push(Value::createInt(0));
            continue;
        }

        OpCode instruction = static_cast<OpCode>(frame.function->chunk[frame.ip++]);

        switch (instruction)
        {
        case OpCode::PUSH_FUNC:
        {
            uint16_t id = read16(frame.function->chunk, frame.ip);
            push(Value::createFunction(functions[id]));
            break;
        }
        case OpCode::CALL:
        {
            int argCount = read16(frame.function->chunk, frame.ip);
            Value callee = pop();

            if (!callee.isFunc())
                throw std::runtime_error("Can only call functions.");
            if (argCount != callee.funcObj->arity)
                throw std::runtime_error("Expected " + std::to_string(callee.funcObj->arity) + " arguments.");

            // Set up the new Call Stack Frame
            frames.push_back(CallFrame(callee.funcObj, stack.size() - argCount));
            scopes.push_back({});

            // CRITICAL FIX: There is no popping loop here anymore!
            // The compiler's injected STORE_VAR instructions will safely pop them.
            break;
        }
        case OpCode::RETURN:
        {
            Value retVal = pop();
            frames.pop_back();
            scopes.pop_back();
            push(retVal);
            break;
        }
        case OpCode::PUSH:
        {
            uint16_t value = read16(frame.function->chunk, frame.ip);
            push(Value::createInt(value));
            break;
        }
        case OpCode::PUSH_BOOL:
        {
            uint16_t value = read16(frame.function->chunk, frame.ip);
            push(Value::createBool(value != 0));
            break;
        }
        case OpCode::POP:
        {
            pop(); // Throw the garbage value into the void
            break;
        }
        case OpCode::ADD:
        case OpCode::SUB:
        case OpCode::MUL:
        case OpCode::DIV:
        {
            Value b = pop();
            Value a = pop();

            if (!a.isInt() || !b.isInt())
                throw std::runtime_error("Math operations require numbers.");
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
            uint16_t id = read16(frame.function->chunk, frame.ip);
            scopes.back()[id] = pop();
            break;
        }
        case OpCode::LOAD_VAR:
        {
            uint16_t id = read16(frame.function->chunk, frame.ip);
            if (scopes.back().count(id))
                push(scopes.back()[id]);
            else if (scopes.front().count(id))
                push(scopes.front()[id]);
            else
                throw std::runtime_error("Undefined variable!");
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
                push(Value::createBool(false));
            else if (a.isInt())
                push(Value::createBool(a.as.number == b.as.number));
            else if (a.isBool())
                push(Value::createBool(a.as.boolean == b.as.boolean));
            else
                push(Value::createBool(a.stringObj == b.stringObj));
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
            uint16_t target = read16(frame.function->chunk, frame.ip);
            Value condition = pop();

            bool isTruthy = false;
            if (condition.isBool())
                isTruthy = condition.as.boolean;
            else if (condition.isInt())
                isTruthy = (condition.as.number != 0);

            if (!isTruthy)
                frame.ip = target;
            break;
        }
        case OpCode::JUMP:
        {
            frame.ip = read16(frame.function->chunk, frame.ip);
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