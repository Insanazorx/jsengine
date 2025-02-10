#pragma once

namespace JSLib {
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
}