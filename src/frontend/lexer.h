#pragma once
#include <string>
#include <vector>
#include "../core/token.h"

class Lexer
{
public:
    // Takes the raw source code string
    Lexer(std::string source);

    // The main engine function that returns the token array
    std::vector<Token> tokenize();

private:
    std::string source;
    std::vector<Token> tokens;

    int start = 0;   // Start of the current lexeme being scanned
    int current = 0; // Current character being looked at
    int line = 1;    // Current line number

    // Helper functions for scanning
    void scanToken();
    char advance();
    bool match(char expected);
    char peek();
    bool isAtEnd();

    // Specific scanners for longer tokens
    void number();
    void identifier();

    // Utility checks
    bool isDigit(char c);
    bool isAlpha(char c);
    bool isAlphaNumeric(char c);

    // Adding tokens to our list
    void addToken(TokenType type);
    void addToken(TokenType type, std::string text);
};