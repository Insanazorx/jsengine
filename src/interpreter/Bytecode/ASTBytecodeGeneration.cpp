#include "../../ast/AST.h"
#include "BytecodeGenerator.h"

namespace js::AST {
    DECLARE_BYTECODE_GENERATOR_UNIT(ASTNode) {

    }

    DECLARE_BYTECODE_GENERATOR_UNIT(ProgramNode) {
        return {};
    }
    DECLARE_BYTECODE_GENERATOR_UNIT(BlockNode) {
        return {};
    }
    DECLARE_BYTECODE_GENERATOR_UNIT(Identifier) {
        return {};
    }
    DECLARE_BYTECODE_GENERATOR_UNIT(Literal) {
        return {};
    }
    DECLARE_BYTECODE_GENERATOR_UNIT(AssignmentExpression) {
        return {};
    }
    DECLARE_BYTECODE_GENERATOR_UNIT(VariableDeclaration) {
        return {};
    }
    DECLARE_BYTECODE_GENERATOR_UNIT(BinaryOperation) {
        return {};
    }
    DECLARE_BYTECODE_GENERATOR_UNIT(BreakStatement) {
        return {};
    }
    DECLARE_BYTECODE_GENERATOR_UNIT(IfStatement) {
        return {};
    }
    DECLARE_BYTECODE_GENERATOR_UNIT(ForStatement) {
        return {};
    }
    DECLARE_BYTECODE_GENERATOR_UNIT(WhileStatement) {
        return {};
    }
    DECLARE_BYTECODE_GENERATOR_UNIT(ReturnStatement) {
        return {};
    }
    DECLARE_BYTECODE_GENERATOR_UNIT(FunctionDeclaration) {
        return {};
    }
    DECLARE_BYTECODE_GENERATOR_UNIT(CallExpression) {
        return {};
    }
}
