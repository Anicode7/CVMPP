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

// The core pipeline execution function
void run(const std::string &code)
{
    try
    {
        Lexer lexer(code);
        std::vector<Token> tokens = lexer.tokenize();

        Parser parser(tokens);
        std::unique_ptr<BlockNode> ast = parser.parse();

        Compiler compiler;
        std::vector<uint8_t> bytecode = compiler.compile(ast.get());

        VM vm;
        vm.execute(bytecode);
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << "\n";
    }
}

// Mode 1: Read and execute a .cvm file
void runFile(const char *path)
{
    std::ifstream file(path);
    if (!file.is_open())
    {
        std::cerr << "Error: Could not open file '" << path << "'\n";
        return;
    }

    // Read the entire file into a string
    std::stringstream buffer;
    buffer << file.rdbuf();
    run(buffer.str());
}

// Mode 2: Interactive Terminal (REPL)
void runPrompt()
{
    std::cout << "CVM++ Interactive Environment (v1.0)\n";
    std::cout << "Type 'exit' to quit.\n";

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

        run(line);
    }
}

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