#include "lexer.h"
#include <unordered_map>
#include <iostream>

// Map to quickly check if a word is a keyword
static std::unordered_map<std::string, TokenType> keywords = {
    {"let", TokenType::LET},
    {"if", TokenType::IF},
    {"else", TokenType::ELSE},
    {"while", TokenType::WHILE},
    {"print", TokenType::PRINT},
    {"input", TokenType::INPUT},
    {"true", TokenType::TRUE_KW},
    {"false", TokenType::FALSE_KW}};

Lexer::Lexer(std::string source) : source(source) {}

std::vector<Token> Lexer::tokenize()
{
    while (!isAtEnd())
    {
        start = current;
        scanToken();
    }
    // Always append an EOF token so the parser knows when to stop
    tokens.push_back(Token(TokenType::END_OF_FILE, "", line));
    return tokens;
}

void Lexer::scanToken()
{
    char c = advance();
    switch (c)
    {
    // Single character tokens
    case '(':
        addToken(TokenType::LPAREN);
        break;
    case ')':
        addToken(TokenType::RPAREN);
        break;
    case '{':
        addToken(TokenType::LBRACE);
        break;
    case '}':
        addToken(TokenType::RBRACE);
        break;
    case '+':
        addToken(TokenType::PLUS);
        break;
    case '-':
        addToken(TokenType::MINUS);
        break;
    case '*':
        addToken(TokenType::STAR);
        break;
    case '/':
        addToken(TokenType::SLASH);
        break;
    case ';':
        addToken(TokenType::SEMICOLON);
        break;

    // One or two character tokens (like = vs ==)
    case '=':
        addToken(match('=') ? TokenType::EQUAL_EQUAL : TokenType::ASSIGN);
        break;
    case '<':
        addToken(TokenType::LESS);
        break;

    // Ignore whitespace
    case ' ':
    case '\r':
    case '\t':
        break;
    case '\n':
        line++;
        break;

    default:
        if (isDigit(c))
        {
            number();
        }
        else if (isAlpha(c))
        {
            identifier();
        }
        else
        {
            std::cerr << "Error: Unexpected character '" << c << "' at line " << line << "\n";
        }
        break;
    }
}

void Lexer::identifier()
{
    while (isAlphaNumeric(peek()))
        advance();

    std::string text = source.substr(start, current - start);

    // Check if it's a keyword, otherwise it's a standard identifier (variable name)
    TokenType type = TokenType::IDENTIFIER;
    if (keywords.find(text) != keywords.end())
    {
        type = keywords[text];
    }
    addToken(type, text);
}

void Lexer::number()
{
    while (isDigit(peek()))
        advance();
    addToken(TokenType::NUMBER, source.substr(start, current - start));
}

// --- Helper Functions ---
bool Lexer::isAtEnd() { return current >= source.length(); }
char Lexer::advance() { return source[current++]; }
char Lexer::peek()
{
    if (isAtEnd())
        return '\0';
    return source[current];
}
bool Lexer::match(char expected)
{
    if (isAtEnd())
        return false;
    if (source[current] != expected)
        return false;
    current++;
    return true;
}
bool Lexer::isDigit(char c) { return c >= '0' && c <= '9'; }
bool Lexer::isAlpha(char c) { return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_'; }
bool Lexer::isAlphaNumeric(char c) { return isAlpha(c) || isDigit(c); }
void Lexer::addToken(TokenType type) { addToken(type, source.substr(start, current - start)); }
void Lexer::addToken(TokenType type, std::string text)
{
    tokens.push_back(Token(type, text, line));
}