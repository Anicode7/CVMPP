#include "compiler.h"
#include <stdexcept>

Compiler::Compiler(std::unordered_map<std::string, uint16_t> &globals, uint16_t &nextId, std::vector<std::shared_ptr<FunctionObj>> &funcs)
    : variables(globals), nextVarId(nextId), functions(funcs) {}

std::vector<uint8_t> Compiler::compile(ASTNode *root, bool isFunction)
{
    bytecode.clear();
    visit(root);

    if (isFunction)
    {
        emit(OpCode::PUSH);
        emit16(0);
        emit(OpCode::RETURN);
    }
    else
    {
        emit(OpCode::HALT);
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
        emit(OpCode::PUSH_BOOL);
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
    else if (auto *v = dynamic_cast<VarDeclNode *>(node))
    {
        visit(v->initializer.get());
        emit(OpCode::STORE_VAR);
        emit16(getVarId(v->varName));
    }
    else if (auto *a = dynamic_cast<AssignNode *>(node))
    {
        visit(a->value.get());
        emit(OpCode::STORE_VAR);
        emit16(getVarId(a->varName));

        emit(OpCode::LOAD_VAR);
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
        emit(OpCode::POP);
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
    else if (auto *ret = dynamic_cast<ReturnNode *>(node))
    {
        if (ret->value)
            visit(ret->value.get());
        else
        {
            emit(OpCode::PUSH);
            emit16(0);
        }
        emit(OpCode::RETURN);
    }
    else if (auto *f = dynamic_cast<FunctionDeclNode *>(node))
    {
        uint16_t funcNameId = getVarId(f->name);
        auto funcObj = std::make_shared<FunctionObj>(f->name, f->params.size());

        std::unordered_map<std::string, uint16_t> localVars = variables;
        uint16_t localId = nextVarId;

        std::vector<uint16_t> paramIds;
        for (const auto &param : f->params)
        {
            uint16_t pid = localId++;
            localVars[param] = pid;
            paramIds.push_back(pid);
        }

        Compiler funcCompiler(localVars, localId, functions);

        for (int i = static_cast<int>(paramIds.size()) - 1; i >= 0; --i)
        {
            funcCompiler.emit(OpCode::STORE_VAR);
            funcCompiler.emit16(paramIds[i]);
        }

        funcCompiler.visit(f->body.get());

        funcCompiler.emit(OpCode::PUSH);
        funcCompiler.emit16(0);
        funcCompiler.emit(OpCode::RETURN);

        funcObj->chunk = funcCompiler.bytecode;

        functions.push_back(funcObj);
        emit(OpCode::PUSH_FUNC);
        emit16(static_cast<uint16_t>(functions.size() - 1));
        emit(OpCode::STORE_VAR);
        emit16(funcNameId);
    }
    else if (auto *call = dynamic_cast<CallNode *>(node))
    {
        for (const auto &arg : call->arguments)
            visit(arg.get());
        visit(call->callee.get());
        emit(OpCode::CALL);
        emit16(static_cast<uint16_t>(call->arguments.size()));
    }
}