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
    // FIX: Constructor now takes the functions pool as the 3rd argument
    Compiler(std::unordered_map<std::string, uint16_t> &globals, uint16_t &nextId, std::vector<std::shared_ptr<FunctionObj>> &funcs);

    std::vector<uint8_t> compile(ASTNode *root, bool isFunction = false);

    const std::vector<std::shared_ptr<FunctionObj>> &getFunctions() const { return functions; }

private:
    std::vector<uint8_t> bytecode;

    // FIX: This MUST be a reference (&) so it shares memory with the VM!
    std::vector<std::shared_ptr<FunctionObj>> &functions;

    std::unordered_map<std::string, uint16_t> &variables;
    uint16_t &nextVarId;

    // ... rest of the file stays the same

    void visit(ASTNode *node);
    void emit(uint8_t byte);
    void emit(OpCode opcode);
    void emit16(uint16_t value);
    void patchJump(int offset, uint16_t target);
    uint16_t getVarId(const std::string &name);
};