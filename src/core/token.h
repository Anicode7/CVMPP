#pragma once
#include <string>

// 1. All the possible "words" and "symbols" in CVM++
enum class TokenType
{
    // Single-character operators and syntax
    LPAREN,    // (
    RPAREN,    // )
    LBRACE,    // {
    RBRACE,    // }
    PLUS,      // +
    MINUS,     // -
    STAR,      // *
    SLASH,     // /
    SEMICOLON, // ;
    ASSIGN,    // =
    LESS,      // <
    COMMA,     // , <-- NEW: For separating function arguments

    // Two-character operators
    EQUAL_EQUAL, // ==

    // Literals (Data)
    IDENTIFIER, // Variable names (e.g., x, y, myVar)
    NUMBER,     // Integers (e.g., 10, 42)

    // Keywords (Reserved words)
    LET,       // let
    IF,        // if
    ELSE,      // else
    WHILE,     // while
    PRINT,     // print
    INPUT,     // input
    TRUE_KW,   // true
    FALSE_KW,  // false
    FN,        // fn <-- NEW: Function declaration
    RETURN_KW, // return <-- NEW: Returning from a function

    // Special Markers
    END_OF_FILE, // Marks the end of the script
    ERROR_TOKEN  // Used if the user types an invalid character
};

// 2. The data structure that holds a single piece of parsed text
struct Token
{
    TokenType type;     // What kind of token is this?
    std::string lexeme; // The actual raw text (e.g., "10", "let", "+")
    int line;           // Which line number is this on? (Great for error messages)

    // A quick constructor so we can create Tokens easily in one line
    Token(TokenType type, std::string lexeme, int line)
        : type(type), lexeme(lexeme), line(line) {}
};