#include "compiler.h"
#include <stdexcept>

std::vector<uint8_t> Compiler::compile(ASTNode *root)
{
    bytecode.clear(); // Ensure we start with a clean slate
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

void Compiler::emit16(uint16_t value)
{
    bytecode.push_back((value >> 8) & 0xFF);
    bytecode.push_back(value & 0xFF);
}

void Compiler::patchJump(int offset, uint16_t target)
{
    bytecode[offset] = (target >> 8) & 0xFF;
    bytecode[offset + 1] = target & 0xFF;
}

uint16_t Compiler::getVarId(const std::string &name)
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

    // 1. Numbers and Booleans
    if (auto *n = dynamic_cast<NumberNode *>(node))
    {
        emit(OpCode::PUSH);
        emit16(static_cast<uint16_t>(n->value)); // 16-bit operand
    }
    else if (auto *boolNode = dynamic_cast<BooleanNode *>(node))
    {
        emit(OpCode::PUSH);
        emit16(boolNode->value ? 1 : 0); // 16-bit operand
    }
    // 2. Input
    else if (dynamic_cast<InputNode *>(node))
    {
        emit(OpCode::INPUT);
    }
    // 3. Binary Operations (Math and Logic)
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
    // 4. Variables (Declare, Assign, Load)
    else if (auto *v = dynamic_cast<VarDeclNode *>(node))
    {
        visit(v->initializer.get());
        emit(OpCode::STORE_VAR);
        emit16(getVarId(v->varName)); // 16-bit operand
    }
    else if (auto *a = dynamic_cast<AssignNode *>(node))
    {
        visit(a->value.get());
        emit(OpCode::STORE_VAR);
        emit16(getVarId(a->varName)); // 16-bit operand
    }
    else if (auto *id = dynamic_cast<IdentifierNode *>(node))
    {
        if (variables.find(id->name) == variables.end())
        {
            throw std::runtime_error("Undefined variable used: " + id->name);
        }
        emit(OpCode::LOAD_VAR);
        emit16(variables[id->name]); // 16-bit operand
    }
    // 5. Printing and Blocks
    else if (auto *p = dynamic_cast<PrintNode *>(node))
    {
        visit(p->expression.get());
        emit(OpCode::PRINT);
    }
    else if (auto *blk = dynamic_cast<BlockNode *>(node))
    {
        for (const auto &stmt : blk->statements)
        {
            visit(stmt.get());
        }
    }
    // 6. Control Flow (If and While)
    // 6. Control Flow (If and While)
    else if (auto *ifStmt = dynamic_cast<IfNode *>(node))
    {
        visit(ifStmt->condition.get()); // Evaluate condition

        emit(OpCode::JUMP_IF_FALSE);
        int jumpTargetIndex = bytecode.size();
        emit16(0xFFFF); // Dummy bytes for the False jump

        visit(ifStmt->thenBranch.get()); // Evaluate 'then' block

        // If there is an 'else' branch, we need to handle jumping around it
        if (ifStmt->elseBranch)
        {
            // Unconditional jump to skip the 'else' block if the 'then' block executes
            emit(OpCode::JUMP);
            int skipElseIndex = bytecode.size();
            emit16(0xFFFF);

            // Patch the JUMP_IF_FALSE to land right here (the start of the 'else' block)
            patchJump(jumpTargetIndex, static_cast<uint16_t>(bytecode.size()));

            visit(ifStmt->elseBranch.get()); // Evaluate 'else' block

            // Patch the unconditional JUMP to land right here (the end of the 'if-else' chain)
            patchJump(skipElseIndex, static_cast<uint16_t>(bytecode.size()));
        }
        else
        {
            // No 'else', so just patch the JUMP_IF_FALSE to land at the end of 'then'
            patchJump(jumpTargetIndex, static_cast<uint16_t>(bytecode.size()));
        }
    }
    else if (auto *whileStmt = dynamic_cast<WhileNode *>(node))
    {
        int loopStartIndex = bytecode.size();

        visit(whileStmt->condition.get());

        emit(OpCode::JUMP_IF_FALSE);
        int exitJumpIndex = bytecode.size();
        emit16(0xFFFF); // 2 dummy bytes

        visit(whileStmt->body.get());

        emit(OpCode::JUMP);
        emit16(static_cast<uint16_t>(loopStartIndex)); // 16-bit operand

        patchJump(exitJumpIndex, static_cast<uint16_t>(bytecode.size()));
    }
}