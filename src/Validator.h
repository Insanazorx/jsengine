#pragma once

#include "ErrorOr.h"
#include "LexerTypes.h"
#include "Forward.h"
namespace JSLib {
    using namespace Util;
    enum class SyntaxErrorType {
        MISSING_BRACKET
    };

    class SyntaxError : public Error {
    public:
        SyntaxError(std::string what, TokenPosition pos) : Error(std::move(what)), m_Position(pos) {}
        SyntaxError(std::string what) : Error(std::move(what)), m_Position({-1, -1}) {}
    private:
        TokenPosition m_Position;
        SyntaxErrorType m_Type;
    };
    class Validator {
    public:
        Validator() = default;
        virtual ~Validator() = default;

        virtual ErrorOr<void, SyntaxError> Expect(TokenType expected);
        virtual ErrorOr<int, SyntaxError> ValidateParenthesesMatchAndReturnLastParenthesesIndex(std::vector<Token>& tokens);
        virtual ErrorOr<int, SyntaxError> ValidateBracketsMatchAndReturnLastBracketIndex(std::vector<Token>& tokens);

        virtual TokenPosition Visit(IfStatement* statement);
        virtual TokenPosition Visit(ForStatement* statement);
        virtual TokenPosition Visit(WhileStatement* statement);
        virtual TokenPosition Visit(ReturnStatement* statement);
        virtual TokenPosition Visit(FunctionStatement* statement);
        virtual TokenPosition Visit(ImmediateStatement* statement);
        virtual TokenPosition Visit(BinaryOpStatement* statement);
        virtual TokenPosition Visit(UnaryOpStatement* statement);
        virtual TokenPosition Visit(AssignmentStatement* statement);
        virtual TokenPosition Visit(VariableDeclarationStatement* statement);
        virtual TokenPosition Visit(ProgramStatement* statement);
        virtual TokenPosition Visit(ScopeStatement* statement);

        virtual TokenPosition Visit(IfNode* node);
        virtual TokenPosition Visit(ForNode* node);
        virtual TokenPosition Visit(WhileNode* node);
        virtual TokenPosition Visit(ReturnNode* node);
        virtual TokenPosition Visit(FunctionNode* node);
        virtual TokenPosition Visit(BinaryOpNode* node);
        virtual TokenPosition Visit(UnaryOpNode* node);
        virtual TokenPosition Visit(AssignmentNode* node);
        virtual TokenPosition Visit(VariableDeclarationNode* node);
        virtual TokenPosition Visit(ScopeNode* node);

    };
    class SyntaxValidator : public Validator {
    public:
        SyntaxValidator() = default;
        ~SyntaxValidator() override = default;

        ErrorOr<void, SyntaxError> Expect(TokenType expected) override;

        TokenPosition Visit(IfStatement* statement) override;
        TokenPosition Visit(ForStatement* statement) override;
        TokenPosition Visit(WhileStatement* statement) override;
        TokenPosition Visit(ReturnStatement* statement) override;
        TokenPosition Visit(FunctionStatement* statement) override;
        TokenPosition Visit(ImmediateStatement* statement) override;
        TokenPosition Visit(BinaryOpStatement* statement) override;
        TokenPosition Visit(UnaryOpStatement* statement) override;
        TokenPosition Visit(AssignmentStatement* statement) override;
        TokenPosition Visit(VariableDeclarationStatement* statement) override;
        TokenPosition Visit(ProgramStatement* statement) override;
        TokenPosition Visit(ScopeStatement* statement) override;

    };
    class SemanticValidator : public Validator {
    public:
        SemanticValidator() = default;
        ~SemanticValidator() override = default;

        TokenPosition Visit(IfNode* node) override;
        TokenPosition Visit(ForNode* node) override;
        TokenPosition Visit(WhileNode* node) override;
        TokenPosition Visit(ReturnNode* node) override;
        TokenPosition Visit(FunctionNode* node) override;
        TokenPosition Visit(BinaryOpNode* node) override;
        TokenPosition Visit(UnaryOpNode* node) override;
        TokenPosition Visit(AssignmentNode* node) override;
        TokenPosition Visit(VariableDeclarationNode* node) override;

    };
}

