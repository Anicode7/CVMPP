#pragma once
#include <vector>
#include <cstdint>
#include <unordered_map>
#include <memory>
#include "../core/opcodes.h"
#include "../core/value.h"

struct CallFrame
{
    std::shared_ptr<FunctionObj> function;
    size_t ip;
    size_t basePointer;

    CallFrame(std::shared_ptr<FunctionObj> func, size_t base = 0)
        : function(func), ip(0), basePointer(base) {}
};

class VM
{
public:
    void execute(const std::vector<uint8_t> &bytecode, const std::vector<std::shared_ptr<FunctionObj>> &compilerFuncs);

    std::vector<std::unordered_map<uint16_t, Value>> scopes;

private:
    std::vector<Value> stack;
    std::vector<CallFrame> frames;
    std::vector<std::shared_ptr<FunctionObj>> functions;

    void push(Value value);
    Value pop();
    uint16_t read16(const std::vector<uint8_t> &bytecode, size_t &ip);
};