#include <iostream>
#include "frontend/lexer.h"
#include "frontend/parser.h"
#include "backend/compiler.h"
#include "backend/vm.h"
#include "core/token.h"
#include "core/ast.h"

int main()
{
    std::string testCode =
        "let myVar = 10 + 5 * 2;\n"
        "print myVar;";

    std::cout << "--- CVM++ Executing ---\n";
    std::cout << "Code:\n"
              << testCode << "\n";
    std::cout << "-----------------------\n";
    std::cout << "Output:\n";

    try
    {
        // 1. Front-End
        Lexer lexer(testCode);
        std::vector<Token> tokens = lexer.tokenize();

        Parser parser(tokens);
        std::unique_ptr<BlockNode> ast = parser.parse();

        // 2. Back-End
        Compiler compiler;
        std::vector<uint8_t> bytecode = compiler.compile(ast.get());

        // 3. Runtime execution
        VM vm;
        vm.execute(bytecode);
    }
    catch (const std::exception &e)
    {
        std::cerr << "\nFatal Error: " << e.what() << "\n";
    }

    return 0;
}