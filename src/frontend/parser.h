#pragma once
#include <vector>
#include <memory>
#include "../core/token.h"
#include "../core/ast.h"

class Parser
{
public:
    Parser(const std::vector<Token> &tokens);

    // Parses the entire token list into a single block of statements
    std::unique_ptr<BlockNode> parse();

private:
    std::vector<Token> tokens;
    int current = 0;

    // --- Grammar Rules ---
    std::unique_ptr<ASTNode> statement();
    std::unique_ptr<ASTNode> declaration();
    std::unique_ptr<ASTNode> printStatement();
    std::unique_ptr<ASTNode> ifStatement();
    std::unique_ptr<ASTNode> whileStatement();
    std::unique_ptr<ASTNode> block();
    std::unique_ptr<ASTNode> expressionStatement();

    // Order of operations for math
    std::unique_ptr<ASTNode> expression();
    std::unique_ptr<ASTNode> assignment();
    std::unique_ptr<ASTNode> equality();
    std::unique_ptr<ASTNode> comparison();
    std::unique_ptr<ASTNode> term();
    std::unique_ptr<ASTNode> factor();
    std::unique_ptr<ASTNode> primary();

    // --- Helper Functions ---
    bool match(std::initializer_list<TokenType> types);
    bool check(TokenType type);
    bool isAtEnd();
    Token advance();
    Token peek();
    Token previous();
    Token consume(TokenType type, const std::string &message);
};