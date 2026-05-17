#include "compiler.h"
#include <stdexcept>

std::vector<uint8_t> Compiler::compile(ASTNode *root)
{
    visit(root);
    emit(OpCode::HALT); // Always tell the VM when to stop
    return bytecode;
}

void Compiler::emit(uint8_t byte)
{
    bytecode.push_back(byte);
}

void Compiler::emit(OpCode opcode)
{
    bytecode.push_back(static_cast<uint8_t>(opcode));
}

uint8_t Compiler::getVarId(const std::string &name)
{
    if (variables.find(name) == variables.end())
    {
        variables[name] = nextVarId++;
    }
    return variables[name];
}

void Compiler::visit(ASTNode *node)
{
    if (!node)
        return;

    // 1. If it's a raw number, emit PUSH and then the number itself
    if (auto *n = dynamic_cast<NumberNode *>(node))
    {
        emit(OpCode::PUSH);
        emit(static_cast<uint8_t>(n->value));
    }

    else if (auto *boolNode = dynamic_cast<BooleanNode *>(node))
    {
        emit(OpCode::PUSH);
        emit(boolNode->value ? 1 : 0); // True is 1, False is 0
    }
    // 2. If it's math, visit children first (post-order), then emit operation
    else if (auto *b = dynamic_cast<BinaryOpNode *>(node))
    {
        visit(b->left.get());
        visit(b->right.get());

        switch (b->op)
        {
        case TokenType::PLUS:
            emit(OpCode::ADD);
            break;
        case TokenType::MINUS:
            emit(OpCode::SUB);
            break;
        case TokenType::STAR:
            emit(OpCode::MUL);
            break;
        case TokenType::SLASH:
            emit(OpCode::DIV);
            break;
        case TokenType::EQUAL_EQUAL:
            emit(OpCode::EQUAL);
            break;
        case TokenType::LESS:
            emit(OpCode::LESS);
            break;
        default:
            break;
        }
    }
    // 3. If declaring or assigning a variable, evaluate the right side, then STORE
    else if (auto *v = dynamic_cast<VarDeclNode *>(node))
    {
        visit(v->initializer.get());
        emit(OpCode::STORE_VAR);
        emit(getVarId(v->varName));
    }
    else if (auto *a = dynamic_cast<AssignNode *>(node))
    {
        visit(a->value.get());
        emit(OpCode::STORE_VAR);
        emit(getVarId(a->varName));
    }
    // 4. If using a variable, LOAD it
    else if (auto *id = dynamic_cast<IdentifierNode *>(node))
    {
        if (variables.find(id->name) == variables.end())
        {
            throw std::runtime_error("Undefined variable used: " + id->name);
        }
        emit(OpCode::LOAD_VAR);
        emit(variables[id->name]);
    }
    // 5. If printing, evaluate the expression, then PRINT
    else if (auto *p = dynamic_cast<PrintNode *>(node))
    {
        visit(p->expression.get());
        emit(OpCode::PRINT);
    }
    // 6. If it's a block of code, just visit every statement inside it
    else if (auto *blk = dynamic_cast<BlockNode *>(node))
    {
        for (const auto &stmt : blk->statements)
        {
            visit(stmt.get());
        }
    }
    // --- CONTROL FLOW ---
    else if (auto *ifStmt = dynamic_cast<IfNode *>(node))
    {
        visit(ifStmt->condition.get()); // Compile the condition

        emit(OpCode::JUMP_IF_FALSE);
        int jumpTargetIndex = bytecode.size(); // Remember where our dummy byte is
        emit(0);                               // Emit dummy byte

        visit(ifStmt->thenBranch.get()); // Compile the body

        // Backpatch! We cast the size to uint8_t (max 255 bytes for now)
        bytecode[jumpTargetIndex] = static_cast<uint8_t>(bytecode.size());
    }
    else if (auto *whileStmt = dynamic_cast<WhileNode *>(node))
    {
        int loopStartIndex = bytecode.size(); // Remember where the loop starts!

        visit(whileStmt->condition.get());

        emit(OpCode::JUMP_IF_FALSE);
        int exitJumpIndex = bytecode.size();
        emit(0); // Dummy byte for loop exit

        visit(whileStmt->body.get());

        // Jump back up to re-evaluate the condition
        emit(OpCode::JUMP);
        emit(static_cast<uint8_t>(loopStartIndex));

        // Backpatch the exit jump!
        bytecode[exitJumpIndex] = static_cast<uint8_t>(bytecode.size());
    }
}
