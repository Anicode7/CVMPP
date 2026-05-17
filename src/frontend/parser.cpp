#include "parser.h"
#include <stdexcept>

Parser::Parser(const std::vector<Token> &tokens) : tokens(tokens) {}

std::unique_ptr<BlockNode> Parser::parse()
{
    auto program = std::make_unique<BlockNode>();
    while (!isAtEnd())
    {
        program->statements.push_back(declaration());
    }
    return program;
}

// --- Statements ---
std::unique_ptr<ASTNode> Parser::declaration()
{
    if (match({TokenType::LET}))
    {
        Token name = consume(TokenType::IDENTIFIER, "Expect variable name.");
        consume(TokenType::ASSIGN, "Expect '=' after variable name.");
        std::unique_ptr<ASTNode> initializer = expression();
        consume(TokenType::SEMICOLON, "Expect ';' after variable declaration.");
        return std::make_unique<VarDeclNode>(name.lexeme, std::move(initializer));
    }
    return statement();
}

std::unique_ptr<ASTNode> Parser::statement()
{
    if (match({TokenType::PRINT}))
        return printStatement();
    return expressionStatement();
}

std::unique_ptr<ASTNode> Parser::printStatement()
{
    std::unique_ptr<ASTNode> expr = expression();
    consume(TokenType::SEMICOLON, "Expect ';' after value.");
    return std::make_unique<PrintNode>(std::move(expr));
}

std::unique_ptr<ASTNode> Parser::expressionStatement()
{
    std::unique_ptr<ASTNode> expr = expression();
    consume(TokenType::SEMICOLON, "Expect ';' after expression.");
    return expr;
}

// --- Expressions ---
std::unique_ptr<ASTNode> Parser::expression()
{
    return assignment();
}

std::unique_ptr<ASTNode> Parser::assignment()
{
    std::unique_ptr<ASTNode> expr = equality();

    if (match({TokenType::ASSIGN}))
    {
        std::unique_ptr<ASTNode> value = assignment(); // recursion for a = b = c

        // Ensure the left side is actually a variable we can assign to
        if (IdentifierNode *id = dynamic_cast<IdentifierNode *>(expr.get()))
        {
            return std::make_unique<AssignNode>(id->name, std::move(value));
        }
        throw std::runtime_error("Invalid assignment target.");
    }
    return expr;
}

std::unique_ptr<ASTNode> Parser::equality()
{
    std::unique_ptr<ASTNode> expr = comparison();
    while (match({TokenType::EQUAL_EQUAL}))
    {
        TokenType op = previous().type;
        std::unique_ptr<ASTNode> right = comparison();
        expr = std::make_unique<BinaryOpNode>(op, std::move(expr), std::move(right));
    }
    return expr;
}

std::unique_ptr<ASTNode> Parser::comparison()
{
    std::unique_ptr<ASTNode> expr = term();
    while (match({TokenType::LESS}))
    {
        TokenType op = previous().type;
        std::unique_ptr<ASTNode> right = term();
        expr = std::make_unique<BinaryOpNode>(op, std::move(expr), std::move(right));
    }
    return expr;
}

std::unique_ptr<ASTNode> Parser::term()
{
    std::unique_ptr<ASTNode> expr = factor();
    while (match({TokenType::PLUS, TokenType::MINUS}))
    {
        TokenType op = previous().type;
        std::unique_ptr<ASTNode> right = factor();
        expr = std::make_unique<BinaryOpNode>(op, std::move(expr), std::move(right));
    }
    return expr;
}

std::unique_ptr<ASTNode> Parser::factor()
{
    std::unique_ptr<ASTNode> expr = primary();
    while (match({TokenType::STAR, TokenType::SLASH}))
    {
        TokenType op = previous().type;
        std::unique_ptr<ASTNode> right = primary();
        expr = std::make_unique<BinaryOpNode>(op, std::move(expr), std::move(right));
    }
    return expr;
}

std::unique_ptr<ASTNode> Parser::primary()
{
    if (match({TokenType::NUMBER}))
    {
        return std::make_unique<NumberNode>(std::stoi(previous().lexeme));
    }
    if (match({TokenType::IDENTIFIER}))
    {
        return std::make_unique<IdentifierNode>(previous().lexeme);
    }
    if (match({TokenType::LPAREN}))
    {
        std::unique_ptr<ASTNode> expr = expression();
        consume(TokenType::RPAREN, "Expect ')' after expression.");
        return expr;
    }
    throw std::runtime_error("Expect expression.");
}

// --- Helpers ---
bool Parser::match(std::initializer_list<TokenType> types)
{
    for (TokenType type : types)
    {
        if (check(type))
        {
            advance();
            return true;
        }
    }
    return false;
}

bool Parser::check(TokenType type)
{
    if (isAtEnd())
        return false;
    return peek().type == type;
}

Token Parser::advance()
{
    if (!isAtEnd())
        current++;
    return previous();
}

bool Parser::isAtEnd() { return peek().type == TokenType::END_OF_FILE; }
Token Parser::peek() { return tokens[current]; }
Token Parser::previous() { return tokens[current - 1]; }

Token Parser::consume(TokenType type, const std::string &message)
{
    if (check(type))
        return advance();
    throw std::runtime_error(message + " Found: " + peek().lexeme);
}