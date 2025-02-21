#pragma once

namespace JSLib {

    template <typename T>
    concept Callable = std::invocable<T>;


    class Statement;
    class IfStatement;
    class ForStatement;
    class WhileStatement;
    class ReturnStatement;
    class FunctionStatement;
    class ImmediateStatement;
    class BinaryOpStatement;
    class UnaryOpStatement;
    class AssignmentStatement;
    class VariableDeclarationStatement;
    class ProgramStatement;
    class ScopeStatement;
    class SingleTokenStatement;

    class ASTNode;
    class IfNode;
    class ForNode;
    class WhileNode;
    class ReturnNode;
    class FunctionNode;
    class BinaryOpNode;
    class UnaryOpNode;
    class AssignmentNode;
    class VariableDeclarationNode;
    class ScopeNode;

    class Parser;
    class Validator;
    class SyntaxValidator;
    class SemanticValidator;

    class Context;
    class ParserContext;

#define CONVERT_INTERFACE(TYPE)                         \
    friend class TYPE;                                  \
    auto ConvertFrom (Statement* statement) -> typename TYPE##*{          \
       auto tokens = statement->m_tokens;                \
       auto new_statement = TYPE##::Create();             \
       new_statement->m_tokens = tokens;                \
       return new_statement;                                     \
    }



    #define ALL_STATEMENTS_FRIENDS \
    friend class Statement;                \
    friend class IfStatement;              \
    friend class ForStatement;             \
    friend class WhileStatement;           \
    friend class ReturnStatement;          \
    friend class FunctionStatement;        \
    friend class ImmediateStatement;       \
    friend class BinaryOpStatement;        \
    friend class UnaryOpStatement;         \
    friend class AssignmentStatement;      \
    friend class VariableDeclarationStatement; \
    friend class ProgramStatement;         \
    friend class ScopeStatement;           \
    friend class ObjectStatement;          \
    friend class BracketStatement;         \
    friend class BraceStatement;           \
    friend class SingleTokenStatement;
    }