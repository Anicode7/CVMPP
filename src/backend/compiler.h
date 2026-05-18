#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include <cstdint>
#include <memory>
#include "../core/ast.h"
#include "../core/opcodes.h"
#include "../core/value.h"

class Compiler
{
public:
    Compiler(std::unordered_map<std::string, uint16_t> &globals, uint16_t &nextId);

    std::vector<uint8_t> compile(ASTNode *root, bool isFunction = false);

    // NEW: Expose the compiled functions to the VM
    const std::vector<std::shared_ptr<FunctionObj>> &getFunctions() const { return functions; }

private:
    std::vector<uint8_t> bytecode;
    std::vector<std::shared_ptr<FunctionObj>> functions; // Constants Pool for functions

    std::unordered_map<std::string, uint16_t> &variables;
    uint16_t &nextVarId;

    void visit(ASTNode *node);
    void emit(uint8_t byte);
    void emit(OpCode opcode);
    void emit16(uint16_t value);
    void patchJump(int offset, uint16_t target);
    uint16_t getVarId(const std::string &name);
};