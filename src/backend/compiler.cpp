#include "compiler.h"
#include <stdexcept>

Compiler::Compiler(std::unordered_map<std::string, uint16_t> &globals, uint16_t &nextId)
    : variables(globals), nextVarId(nextId) {}

std::vector<uint8_t> Compiler::compile(ASTNode *root, bool isFunction)
{
    bytecode.clear();
    visit(root);

    // FIX: Safely handle implicit returns
    if (isFunction)
    {
        emit(OpCode::PUSH); // Push a default return value (0)
        emit16(0);
        emit(OpCode::RETURN); // Safely return it
    }
    else
    {
        emit(OpCode::HALT); // Main script still halts
    }

    return bytecode;
}
void Compiler::emit(uint8_t byte) { bytecode.push_back(byte); }
void Compiler::emit(OpCode opcode) { bytecode.push_back(static_cast<uint8_t>(opcode)); }
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

    if (auto *n = dynamic_cast<NumberNode *>(node))
    {
        emit(OpCode::PUSH);
        emit16(static_cast<uint16_t>(n->value));
    }
    else if (auto *boolNode = dynamic_cast<BooleanNode *>(node))
    {
        emit(OpCode::PUSH_BOOL); // <-- FIXED
        emit16(boolNode->value ? 1 : 0);
    }
    else if (dynamic_cast<InputNode *>(node))
    {
        emit(OpCode::INPUT);
    }
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
    else if (auto *u = dynamic_cast<UnaryOpNode *>(node))
    {
        visit(u->right.get());
        if (u->op == TokenType::MINUS)
            emit(OpCode::NEGATE);
    }
    else if (auto *a = dynamic_cast<AssignNode *>(node))
    {
        visit(a->value.get());
        emit(OpCode::STORE_VAR);
        emit16(getVarId(a->varName));

        // --- THIS IS THE MISSING MAGIC ---
        // Assignment is an expression, leave a copy of the value on the stack!
        emit(OpCode::LOAD_VAR);
        emit16(getVarId(a->varName));
        // ---------------------------------
    }
    else if (auto *a = dynamic_cast<AssignNode *>(node))
    {
        visit(a->value.get());
        emit(OpCode::STORE_VAR);
        emit16(getVarId(a->varName));
    }
    else if (auto *id = dynamic_cast<IdentifierNode *>(node))
    {
        if (variables.find(id->name) == variables.end())
        {
            throw std::runtime_error("Undefined variable used: " + id->name);
        }
        emit(OpCode::LOAD_VAR);
        emit16(variables[id->name]);
    }
    else if (auto *p = dynamic_cast<PrintNode *>(node))
    {
        visit(p->expression.get());
        emit(OpCode::PRINT);
    }
    else if (auto *exprStmt = dynamic_cast<ExprStmtNode *>(node))
    {
        visit(exprStmt->expr.get());
        emit(OpCode::POP); // <-- FIXED: Throws away the unused value so the stack doesn't leak
    }
    else if (auto *blk = dynamic_cast<BlockNode *>(node))
    {
        for (const auto &stmt : blk->statements)
        {
            visit(stmt.get());
        }
    }
    else if (auto *ifStmt = dynamic_cast<IfNode *>(node))
    {
        visit(ifStmt->condition.get());

        emit(OpCode::JUMP_IF_FALSE);
        int jumpTargetIndex = bytecode.size();
        emit16(0xFFFF);

        visit(ifStmt->thenBranch.get());

        if (ifStmt->elseBranch)
        {
            emit(OpCode::JUMP);
            int skipElseIndex = bytecode.size();
            emit16(0xFFFF);

            patchJump(jumpTargetIndex, static_cast<uint16_t>(bytecode.size()));
            visit(ifStmt->elseBranch.get());
            patchJump(skipElseIndex, static_cast<uint16_t>(bytecode.size()));
        }
        else
        {
            patchJump(jumpTargetIndex, static_cast<uint16_t>(bytecode.size()));
        }
    }
    else if (auto *whileStmt = dynamic_cast<WhileNode *>(node))
    {
        int loopStartIndex = bytecode.size();
        visit(whileStmt->condition.get());

        emit(OpCode::JUMP_IF_FALSE);
        int exitJumpIndex = bytecode.size();
        emit16(0xFFFF);

        visit(whileStmt->body.get());

        emit(OpCode::JUMP);
        emit16(static_cast<uint16_t>(loopStartIndex));
        patchJump(exitJumpIndex, static_cast<uint16_t>(bytecode.size()));
    }
    // --- NEW: Return Statements ---
    else if (auto *ret = dynamic_cast<ReturnNode *>(node))
    {
        if (ret->value)
        {
            visit(ret->value.get()); // Compile the math/variable to return
        }
        else
        {
            // If they just typed `return;`, push a default 0
            emit(OpCode::PUSH);
            emit16(0);
        }
        emit(OpCode::RETURN);
    }
    // --- NEW: User Defined Functions ---
    else if (auto *f = dynamic_cast<FunctionDeclNode *>(node))
    {
        auto funcObj = std::make_shared<FunctionObj>(f->name, f->params.size());

        std::unordered_map<std::string, uint16_t> localVars;
        uint16_t localId = 0;
        for (const auto &param : f->params)
        {
            localVars[param] = localId++;
        }

        Compiler funcCompiler(localVars, localId);
        // FIX: Pass 'true' so the function chunk ends with a RETURN
        funcObj->chunk = funcCompiler.compile(f->body.get(), true);

        functions.push_back(funcObj);
        emit(OpCode::PUSH_FUNC);
        emit16(static_cast<uint16_t>(functions.size() - 1));
        emit(OpCode::STORE_VAR);
        emit16(getVarId(f->name));
    }
    else if (auto *call = dynamic_cast<CallNode *>(node))
    {
        for (const auto &arg : call->arguments)
        {
            visit(arg.get());
        }
        visit(call->callee.get());
        emit(OpCode::CALL);
        emit16(static_cast<uint16_t>(call->arguments.size()));
    }
}