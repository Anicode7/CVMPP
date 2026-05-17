#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "frontend/lexer.h"
#include "frontend/parser.h"
#include "backend/compiler.h"
#include "backend/vm.h"
#include "core/token.h"
#include "core/ast.h"

// The pipeline now accepts boolean flags for the debug modes
void run(const std::string &code, Compiler &compiler, VM &vm, bool showAst, bool showBytecode)
{
    try
    {
        Lexer lexer(code);
        std::vector<Token> tokens = lexer.tokenize();

        Parser parser(tokens);
        std::unique_ptr<BlockNode> ast = parser.parse();

        // DELIVERABLE: Print the Abstract Syntax Tree
        if (showAst)
        {
            std::cout << "\n--- Abstract Syntax Tree ---\n";
            ast->print(0);
            std::cout << "----------------------------\n";
        }

        std::vector<uint8_t> bytecode = compiler.compile(ast.get());

        // DELIVERABLE: Print the Compiled Bytecode (casted to int so it shows numbers, not ASCII chars)
        if (showBytecode)
        {
            std::cout << "\n--- Compiled Bytecode ---\n";
            for (size_t i = 0; i < bytecode.size(); ++i)
            {
                std::cout << static_cast<int>(bytecode[i]) << " ";
            }
            std::cout << "\n-------------------------\n";
        }

        vm.execute(bytecode);
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << "\n";
    }
}

void runFile(const char *path, bool showAst, bool showBytecode)
{
    std::ifstream file(path);
    if (!file.is_open())
    {
        std::cerr << "Error: Could not open file '" << path << "'\n";
        return;
    }
    std::stringstream buffer;
    buffer << file.rdbuf();

    Compiler compiler;
    VM vm;
    run(buffer.str(), compiler, vm, showAst, showBytecode);
}

void runPrompt(bool showAst, bool showBytecode)
{
    std::cout << "CVM++ Interactive Environment (v1.0)\n";
    std::cout << "Type 'exit' to quit.\n";

    Compiler persistentCompiler;
    VM persistentVM;

    std::string line;
    while (true)
    {
        std::cout << "> ";
        if (!std::getline(std::cin, line))
            break;
        if (line == "exit")
            break;
        if (line.empty())
            continue;

        run(line, persistentCompiler, persistentVM, showAst, showBytecode);
    }
}

// The Argument Parser
int main(int argc, char *argv[])
{
    bool showAst = false;
    bool showBytecode = false;
    std::string filePath = "";

    // Loop through the command line arguments
    for (int i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];
        if (arg == "--ast")
        {
            showAst = true;
        }
        else if (arg == "--bytecode")
        {
            showBytecode = true;
        }
        else
        {
            filePath = arg; // If it's not a flag, assume it's the .cvm file
        }
    }

    if (!filePath.empty())
    {
        runFile(filePath.c_str(), showAst, showBytecode);
    }
    else
    {
        runPrompt(showAst, showBytecode);
    }

    return 0;
}