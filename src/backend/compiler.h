#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include <cstdint>
#include "../core/ast.h"
#include "../core/opcodes.h"

class Compiler
{
public:
    // Takes the root of the AST and returns the flat bytecode array
    std::vector<uint8_t> compile(ASTNode *root);

private:
    std::vector<uint8_t> bytecode;

    // CHANGED: Maps variable names to a 16-bit memory slot ID (up to 65,535 variables)
    std::unordered_map<std::string, uint16_t> variables;
    uint16_t nextVarId = 0;

    // The recursive DFS traversal function
    void visit(ASTNode *node);

    // Helpers to push bytes into our array
    void emit(uint8_t byte);
    void emit(OpCode opcode);

    // NEW HELPERS: For 16-bit operands and jump patching
    void emit16(uint16_t value);
    void patchJump(int offset, uint16_t target);

    // CHANGED: Helper to register/lookup variables now returns a 16-bit ID
    uint16_t getVarId(const std::string &name);
};