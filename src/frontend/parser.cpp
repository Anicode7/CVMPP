#include "parser.h"
#include <stdexcept>
#include <iostream>

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
    if (match({TokenType::FN}))
    {
        return functionDeclaration();
    }
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
    if (match({TokenType::IF}))
        return ifStatement();
    if (match({TokenType::WHILE}))
        return whileStatement();
    if (match({TokenType::RETURN_KW}))
        return returnStatement(); // Handles 'return'
    if (match({TokenType::LBRACE}))
        return block();
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
    return std::make_unique<ExprStmtNode>(std::move(expr)); // <-- FIXED
}
// --- THE MISSING LINK: Return Statement Implementation ---
std::unique_ptr<ASTNode> Parser::returnStatement()
{
    std::unique_ptr<ASTNode> value = nullptr;
    if (!check(TokenType::SEMICOLON))
    {
        value = expression();
    }
    consume(TokenType::SEMICOLON, "Expect ';' after return value.");
    return std::make_unique<ReturnNode>(std::move(value));
}
// ---------------------------------------------------------

std::unique_ptr<BlockNode> Parser::block()
{
    auto blockNode = std::make_unique<BlockNode>();
    while (!check(TokenType::RBRACE) && !isAtEnd())
    {
        blockNode->statements.push_back(declaration());
    }
    consume(TokenType::RBRACE, "Expect '}' after block.");
    return blockNode;
}

std::unique_ptr<ASTNode> Parser::ifStatement()
{
    consume(TokenType::LPAREN, "Expect '(' after 'if'.");
    std::unique_ptr<ASTNode> condition = expression();
    consume(TokenType::RPAREN, "Expect ')' after if condition.");

    std::unique_ptr<ASTNode> thenBranch = statement();
    std::unique_ptr<ASTNode> elseBranch = nullptr;

    if (match({TokenType::ELSE}))
    {
        elseBranch = statement();
    }
    return std::make_unique<IfNode>(std::move(condition), std::move(thenBranch), std::move(elseBranch));
}

std::unique_ptr<ASTNode> Parser::whileStatement()
{
    consume(TokenType::LPAREN, "Expect '(' after 'while'.");
    std::unique_ptr<ASTNode> condition = expression();
    consume(TokenType::RPAREN, "Expect ')' after while condition.");

    std::unique_ptr<ASTNode> body = statement();
    return std::make_unique<WhileNode>(std::move(condition), std::move(body));
}

std::unique_ptr<ASTNode> Parser::functionDeclaration()
{
    Token name = consume(TokenType::IDENTIFIER, "Expect function name.");
    consume(TokenType::LPAREN, "Expect '(' after function name.");

    std::vector<std::string> params;
    if (!check(TokenType::RPAREN))
    {
        do
        {
            params.push_back(consume(TokenType::IDENTIFIER, "Expect parameter name.").lexeme);
        } while (match({TokenType::COMMA}));
    }
    consume(TokenType::RPAREN, "Expect ')' after parameters.");
    consume(TokenType::LBRACE, "Expect '{' before function body.");

    return std::make_unique<FunctionDeclNode>(name.lexeme, params, block());
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
        std::unique_ptr<ASTNode> value = assignment();
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
    std::unique_ptr<ASTNode> expr = unary();
    while (match({TokenType::STAR, TokenType::SLASH}))
    {
        TokenType op = previous().type;
        std::unique_ptr<ASTNode> right = unary();
        expr = std::make_unique<BinaryOpNode>(op, std::move(expr), std::move(right));
    }
    return expr;
}

std::unique_ptr<ASTNode> Parser::unary()
{
    if (match({TokenType::MINUS}))
    {
        TokenType op = previous().type;
        std::unique_ptr<ASTNode> right = unary();
        return std::make_unique<UnaryOpNode>(op, std::move(right));
    }
    return primary();
}

std::unique_ptr<ASTNode> Parser::primary()
{
    if (match({TokenType::TRUE_KW}))
        return std::make_unique<BooleanNode>(true);
    if (match({TokenType::FALSE_KW}))
        return std::make_unique<BooleanNode>(false);
    if (match({TokenType::INPUT}))
        return std::make_unique<InputNode>();

    if (match({TokenType::NUMBER}))
    {
        return std::make_unique<NumberNode>(std::stoi(previous().lexeme));
    }

    if (match({TokenType::IDENTIFIER}))
    {
        std::string name = previous().lexeme;

        if (match({TokenType::LPAREN}))
        {
            std::vector<std::unique_ptr<ASTNode>> args;
            if (!check(TokenType::RPAREN))
            {
                do
                {
                    args.push_back(expression());
                } while (match({TokenType::COMMA}));
            }
            consume(TokenType::RPAREN, "Expect ')' after arguments.");
            return std::make_unique<CallNode>(std::make_unique<IdentifierNode>(name), std::move(args));
        }
        return std::make_unique<IdentifierNode>(name);
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
    throw std::runtime_error(message + " Found: '" + peek().lexeme + "'");
}