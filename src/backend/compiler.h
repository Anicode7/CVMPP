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
    // NEW: Constructor takes the global symbol table and ID counter by reference
    Compiler(std::unordered_map<std::string, uint16_t> &globals, uint16_t &nextId);

    // Takes the root of the AST and returns the flat bytecode array
    std::vector<uint8_t> compile(ASTNode *root);

private:
    std::vector<uint8_t> bytecode;

    // CHANGED: These are now references to the persistent state held in main.cpp
    std::unordered_map<std::string, uint16_t> &variables;
    uint16_t &nextVarId;

    // The recursive DFS traversal function
    void visit(ASTNode *node);

    // Helpers to push bytes into our array
    void emit(uint8_t byte);
    void emit(OpCode opcode);

    // Helpers for 16-bit operands and jump patching
    void emit16(uint16_t value);
    void patchJump(int offset, uint16_t target);

    // Helper to register/lookup variables
    uint16_t getVarId(const std::string &name);
};