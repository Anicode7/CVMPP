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

    // Maps variable names (like "myVar") to a memory slot ID (like 0, 1, 2)
    std::unordered_map<std::string, uint8_t> variables;
    uint8_t nextVarId = 0;

    // The recursive DFS traversal function
    void visit(ASTNode *node);

    // Helpers to push bytes into our array
    void emit(uint8_t byte);
    void emit(OpCode opcode);

    // Helper to register/lookup variables
    uint8_t getVarId(const std::string &name);
};