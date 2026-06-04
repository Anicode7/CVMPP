#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <memory> // NEW: For std::shared_ptr

#include "../src/frontend/lexer.h"
#include "../src/frontend/parser.h"
#include "../src/backend/compiler.h"
#include "../src/backend/vm.h"
#include "../src/core/ast.h"
#include "../src/core/opcodes.h"
#include "../src/core/token.h"

namespace
{
    std::string tokenTypeToString(TokenType type)
    {
        switch (type)
        {
        case TokenType::LPAREN:
            return "LPAREN";
        case TokenType::RPAREN:
            return "RPAREN";
        case TokenType::LBRACE:
            return "LBRACE";
        case TokenType::RBRACE:
            return "RBRACE";
        case TokenType::PLUS:
            return "PLUS";
        case TokenType::MINUS:
            return "MINUS";
        case TokenType::STAR:
            return "STAR";
        case TokenType::SLASH:
            return "SLASH";
        case TokenType::SEMICOLON:
            return "SEMICOLON";
        case TokenType::ASSIGN:
            return "ASSIGN";
        case TokenType::LESS:
            return "LESS";
        case TokenType::COMMA:
            return "COMMA"; // NEW
        case TokenType::EQUAL_EQUAL:
            return "EQUAL_EQUAL";
        case TokenType::IDENTIFIER:
            return "IDENTIFIER";
        case TokenType::NUMBER:
            return "NUMBER";
        case TokenType::LET:
            return "LET";
        case TokenType::IF:
            return "IF";
        case TokenType::ELSE:
            return "ELSE";
        case TokenType::WHILE:
            return "WHILE";
        case TokenType::PRINT:
            return "PRINT";
        case TokenType::INPUT:
            return "INPUT";
        case TokenType::TRUE_KW:
            return "TRUE_KW";
        case TokenType::FALSE_KW:
            return "FALSE_KW";
        case TokenType::FN:
            return "FN"; // NEW
        case TokenType::RETURN_KW:
            return "RETURN_KW"; // NEW
        case TokenType::END_OF_FILE:
            return "END_OF_FILE";
        case TokenType::ERROR_TOKEN:
            return "ERROR_TOKEN";
        default:
            return "UNKNOWN_TOKEN";
        }
    }

    std::string opCodeToString(OpCode opcode)
    {
        switch (opcode)
        {
        case OpCode::PUSH:
            return "PUSH";
        case OpCode::PUSH_BOOL:
            return "PUSH_BOOL"; // NEW
        case OpCode::POP:
            return "POP"; // NEW
        case OpCode::ADD:
            return "ADD";
        case OpCode::SUB:
            return "SUB";
        case OpCode::MUL:
            return "MUL";
        case OpCode::DIV:
            return "DIV";
        case OpCode::NEGATE:
            return "NEGATE";
        case OpCode::STORE_VAR:
            return "STORE_VAR";
        case OpCode::LOAD_VAR:
            return "LOAD_VAR";
        case OpCode::PRINT:
            return "PRINT";
        case OpCode::HALT:
            return "HALT";
        case OpCode::EQUAL:
            return "EQUAL";
        case OpCode::LESS:
            return "LESS";
        case OpCode::JUMP_IF_FALSE:
            return "JUMP_IF_FALSE";
        case OpCode::JUMP:
            return "JUMP";
        case OpCode::INPUT:
            return "INPUT";
        case OpCode::PUSH_FUNC:
            return "PUSH_FUNC"; // NEW
        case OpCode::CALL:
            return "CALL"; // NEW
        case OpCode::RETURN:
            return "RETURN"; // NEW
        default:
            return "UNKNOWN_OPCODE";
        }
    }

    bool hasOperand16(OpCode opcode)
    {
        // UPDATED: Added new Phase 3 opcodes that require 16-bit operands
        return opcode == OpCode::PUSH || opcode == OpCode::STORE_VAR || opcode == OpCode::LOAD_VAR ||
               opcode == OpCode::JUMP_IF_FALSE || opcode == OpCode::JUMP ||
               opcode == OpCode::PUSH_BOOL || opcode == OpCode::PUSH_FUNC || opcode == OpCode::CALL;
    }

    uint16_t read16(const std::vector<uint8_t> &bytecode, size_t index)
    {
        return static_cast<uint16_t>((bytecode[index] << 8) | bytecode[index + 1]);
    }

    void printTokenTable(const std::vector<Token> &tokens)
    {
        std::cout << "\n=== TOKENS ===\n";
        for (size_t i = 0; i < tokens.size(); ++i)
        {
            const Token &t = tokens[i];
            std::cout << std::setw(3) << i << " | line " << std::setw(3) << t.line << " | "
                      << std::setw(14) << tokenTypeToString(t.type) << " | '" << t.lexeme << "'\n";
        }
    }

    void printSymbolTable(const std::unordered_map<std::string, uint16_t> &globals)
    {
        std::cout << "\n=== SYMBOL TABLE ===\n";
        if (globals.empty())
        {
            std::cout << "(empty)\n";
            return;
        }

        for (const auto &entry : globals)
        {
            std::cout << entry.first << " -> " << entry.second << "\n";
        }
    }

    void printBytecode(const std::vector<uint8_t> &bytecode)
    {
        std::cout << "\n=== BYTECODE (RAW BYTES) ===\n";
        for (size_t i = 0; i < bytecode.size(); ++i)
        {
            std::cout << static_cast<int>(bytecode[i]) << (i + 1 < bytecode.size() ? ' ' : '\n');
        }

        std::cout << "\n=== BYTECODE (DISASSEMBLED) ===\n";
        size_t ip = 0;
        while (ip < bytecode.size())
        {
            size_t start = ip;
            OpCode opcode = static_cast<OpCode>(bytecode[ip++]);
            std::cout << std::setw(3) << start << " | " << opCodeToString(opcode);

            if (hasOperand16(opcode))
            {
                if (ip + 1 >= bytecode.size())
                {
                    std::cout << " <truncated operand>\n";
                    break;
                }
                uint16_t operand = read16(bytecode, ip);
                std::cout << " " << operand;
                ip += 2;
            }

            std::cout << "\n";
        }
    }

    std::string readProgramFromFile(const std::string &path)
    {
        std::ifstream file(path);
        if (!file.is_open())
        {
            throw std::runtime_error("Could not open file: " + path);
        }
        std::stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    }

    std::string readProgramFromStdin()
    {
        std::cout << "Paste your CVM++ program below. Type END on a new line to run.\n";
        std::stringstream input;
        std::string line;
        while (std::getline(std::cin, line))
        {
            if (line == "END")
            {
                break;
            }
            input << line << '\n';
        }
        return input.str();
    }
} // namespace

int main(int argc, char *argv[])
{
    try
    {
        std::string source;
        if (argc > 1)
        {
            source = readProgramFromFile(argv[1]);
        }
        else
        {
            source = readProgramFromStdin();
        }

        std::cout << "\n=== SOURCE ===\n"
                  << source << "\n";

        Lexer lexer(source);
        std::vector<Token> tokens = lexer.tokenize();
        printTokenTable(tokens);

        Parser parser(tokens);
        std::unique_ptr<BlockNode> ast = parser.parse();

        std::cout << "\n=== AST ===\n";
        ast->print(0);

        std::unordered_map<std::string, uint16_t> globals;
        uint16_t nextVarId = 0;
        std::vector<std::shared_ptr<FunctionObj>> functions; // NEW: Functions Pool

        // UPDATED: Pass functions to compiler
        Compiler compiler(globals, nextVarId, functions);
        std::vector<uint8_t> bytecode = compiler.compile(ast.get());

        printSymbolTable(globals);
        printBytecode(bytecode);

        std::cout << "\n=== VM OUTPUT ===\n";
        VM vm;
        // UPDATED: Pass functions to VM
        vm.execute(bytecode, functions);
        std::cout << "=== END ===\n";
    }
    catch (const std::exception &e)
    {
        std::cerr << "Visualizer Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}