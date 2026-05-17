#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "frontend/lexer.h"
#include "frontend/parser.h"
#include "backend/compiler.h"
#include "backend/vm.h"
#include "core/token.h"
#include "core/ast.h"

// Notice we now pass the Compiler and VM in by reference (&) so they survive
void run(const std::string &code, Compiler &compiler, VM &vm)
{
    try
    {
        Lexer lexer(code);
        std::vector<Token> tokens = lexer.tokenize();

        Parser parser(tokens);
        std::unique_ptr<BlockNode> ast = parser.parse();

        // Compile the new AST, but keep the old variable ID map!
        std::vector<uint8_t> bytecode = compiler.compile(ast.get());

        // Execute the new bytecode, but keep the old memory state!
        vm.execute(bytecode);
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << "\n";
    }
}

// Mode 1: File Execution
void runFile(const char *path)
{
    std::ifstream file(path);
    if (!file.is_open())
    {
        std::cerr << "Error: Could not open file '" << path << "'\n";
        return;
    }
    std::stringstream buffer;
    buffer << file.rdbuf();

    // Create one-time instances for the file execution
    Compiler compiler;
    VM vm;
    run(buffer.str(), compiler, vm);
}

// Mode 2: Interactive Terminal
void runPrompt()
{
    std::cout << "CVM++ Interactive Environment (v1.0)\n";
    std::cout << "Type 'exit' to quit.\n";

    // Create PERSISTENT instances that live as long as the REPL is open
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

        // Pass them into the run function
        run(line, persistentCompiler, persistentVM);
    }
}

// THE MISSING PIECE: The actual starting point of the C++ program
int main(int argc, char *argv[])
{
    // If the user passes a file: ./cvm examples/loop_test.cvm
    if (argc == 2)
    {
        runFile(argv[1]);
    }
    // If the user passes too many arguments
    else if (argc > 2)
    {
        std::cout << "Usage: cvm [script.cvm]\n";
        return 64;
    }
    // If the user just types: ./cvm
    else
    {
        runPrompt();
    }
    return 0;
}