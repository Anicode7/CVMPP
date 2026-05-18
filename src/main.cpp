#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map> // NEW
#include "frontend/lexer.h"
#include "frontend/parser.h"
#include "backend/compiler.h"
#include "backend/vm.h"
#include "core/token.h"
#include "core/ast.h"

// The pipeline now requires the global state to be passed in
void run(const std::string &code, std::unordered_map<std::string, uint16_t> &globals, uint16_t &nextVarId, VM &vm, bool showAst, bool showBytecode)
{
    try
    {
        Lexer lexer(code);
        std::vector<Token> tokens = lexer.tokenize();

        Parser parser(tokens);
        std::unique_ptr<BlockNode> ast = parser.parse();

        if (showAst)
        {
            std::cout << "\n--- Abstract Syntax Tree ---\n";
            ast->print(0);
            std::cout << "----------------------------\n";
        }

        // Spawn a FRESH compiler for this specific execution, linked to our persistent globals
        Compiler compiler(globals, nextVarId);
        std::vector<uint8_t> bytecode = compiler.compile(ast.get());

        if (showBytecode)
        {
            std::cout << "\n--- Compiled Bytecode ---\n";
            for (size_t i = 0; i < bytecode.size(); ++i)
            {
                std::cout << static_cast<int>(bytecode[i]) << " ";
            }
            std::cout << "\n-------------------------\n";
        }

        vm.execute(bytecode, compiler.getFunctions());
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

    // Isolated state for file execution
    std::unordered_map<std::string, uint16_t> globals;
    uint16_t nextVarId = 0;
    VM vm;

    run(buffer.str(), globals, nextVarId, vm, showAst, showBytecode);
}

void runPrompt(bool showAst, bool showBytecode)
{
    std::cout << "CVM++ Interactive Environment (v1.0)\n";
    std::cout << "Type 'exit' to quit.\n";

    // This is the true Global State that persists between REPL lines
    std::unordered_map<std::string, uint16_t> globals;
    uint16_t nextVarId = 0;
    VM persistentVM;

    std::string line;
    while (true)
    {
        std::cout << "> ";
        // Using getline ensures spaces don't break the input!
        if (!std::getline(std::cin, line))
            break;
        if (line == "exit")
            break;
        if (line.empty())
            continue;

        run(line, globals, nextVarId, persistentVM, showAst, showBytecode);
    }
}

// The Argument Parser
int main(int argc, char *argv[])
{
    bool showAst = false;
    bool showBytecode = false;
    std::string filePath = "";

    for (int i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];
        if (arg == "--ast")
            showAst = true;
        else if (arg == "--bytecode")
            showBytecode = true;
        else
            filePath = arg;
    }

    if (!filePath.empty())
        runFile(filePath.c_str(), showAst, showBytecode);
    else
        runPrompt(showAst, showBytecode);

    return 0;
}