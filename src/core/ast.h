#pragma once
#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include "token.h"

// The base class that all nodes inherit from
struct ASTNode
{
    virtual ~ASTNode() = default;
    // A helper to visually print the tree in the terminal
    virtual void print(int indent = 0) const = 0;
};

// --- LEAF NODES (Values) ---
struct NumberNode : public ASTNode
{
    int value;
    NumberNode(int val) : value(val) {}
    void print(int indent) const override
    {
        std::cout << std::string(indent, ' ') << "Number: " << value << "\n";
    }
};

struct IdentifierNode : public ASTNode
{
    std::string name;
    IdentifierNode(std::string n) : name(n) {}
    void print(int indent) const override
    {
        std::cout << std::string(indent, ' ') << "Identifier: " << name << "\n";
    }
};

// --- EXPRESSION NODES (Math & Logic) ---
struct BinaryOpNode : public ASTNode
{
    TokenType op;
    std::unique_ptr<ASTNode> left;
    std::unique_ptr<ASTNode> right;

    BinaryOpNode(TokenType o, std::unique_ptr<ASTNode> l, std::unique_ptr<ASTNode> r)
        : op(o), left(std::move(l)), right(std::move(r)) {}

    void print(int indent) const override
    {
        std::cout << std::string(indent, ' ') << "BinaryOp: " << (int)op << "\n";
        left->print(indent + 2);
        right->print(indent + 2);
    }
};

// --- STATEMENT NODES (Actions) ---
struct VarDeclNode : public ASTNode
{
    std::string varName;
    std::unique_ptr<ASTNode> initializer;

    VarDeclNode(std::string name, std::unique_ptr<ASTNode> init)
        : varName(name), initializer(std::move(init)) {}

    void print(int indent) const override
    {
        std::cout << std::string(indent, ' ') << "Declare: " << varName << " =\n";
        initializer->print(indent + 2);
    }
};

struct AssignNode : public ASTNode
{
    std::string varName;
    std::unique_ptr<ASTNode> value;

    AssignNode(std::string name, std::unique_ptr<ASTNode> val)
        : varName(name), value(std::move(val)) {}

    void print(int indent) const override
    {
        std::cout << std::string(indent, ' ') << "Assign: " << varName << " =\n";
        value->print(indent + 2);
    }
};

struct PrintNode : public ASTNode
{
    std::unique_ptr<ASTNode> expression;

    PrintNode(std::unique_ptr<ASTNode> expr) : expression(std::move(expr)) {}

    void print(int indent) const override
    {
        std::cout << std::string(indent, ' ') << "Print:\n";
        expression->print(indent + 2);
    }
};

struct BlockNode : public ASTNode
{
    std::vector<std::unique_ptr<ASTNode>> statements;

    void print(int indent) const override
    {
        std::cout << std::string(indent, ' ') << "Block:\n";
        for (const auto &stmt : statements)
        {
            stmt->print(indent + 2);
        }
    }
};