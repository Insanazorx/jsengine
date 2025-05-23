#include "Parser.h"

#include <future>

#include "../ast/AST.h"
#include "debug.h"

#include <variant>
#include <optional>
#include <string>


namespace js {
Parser* Parser::m_Instance = nullptr;

std::variant<ASTNode *, Statement *> Parser::AnalyzeImpl(std::optional<std::vector<Token> > tokens,
                                                         ParserContext *context, AnalyzeMode mode) {

            context->PushCallStack(FUNCTION_NAME());

            VERIFY_ONCE(!tokens,"Tokens should not provided at first");

            if (!tokens) {
                m_CurrentTokenCounter = m_MajorTokenCounter;
            } else {
                m_MinorTokenCounter = MinorTokenCounter::Create();
                m_CurrentTokenCounter = m_MinorTokenCounter;
                m_CurrentTokenCounter->SetTokens(tokens.value());
            }

            VERIFY_ONCE(m_MajorTokenCounter->Type() == TokenCounterType::MAJOR, "MajorTokenCounter should be selected first");

        ASTNode* RetNode = nullptr;
        Statement* RetStatement = nullptr;
        //consume all minor tokens consumed already

        while (PeekFront(0) != TokenType::END_OF_STREAM) {

            if (tokenCounter()->Type() == TokenCounterType::MAJOR)
                for (;Context()->ConsumeCountFromMinorCounters() > 0; Context()->decConsumeCount());

            auto new_statement = Statement::Create();

            if (DecideIfSingleTokenStatement()) {
                new_statement = SingleTokenStatement::Create(Consume());
            }

            switch (GetExpressionScaffoldingTypeByLookingKeyword(PeekFront(0))) {

                case ExpressionScaffoldingType::IF_EXPR:
                    RulesForIfExpr(new_statement);
                break;
                case ExpressionScaffoldingType::EXPR_WITH_BRACKETS_AND_BRACES:
                    RulesForExprWithBracketsAndBraces(new_statement);
                break;
                case ExpressionScaffoldingType::EXPR_WITH_BRACKETS_AND_OPTIONAL_BRACES:
                    RulesForExprWithBracketsAndOptionalBraces(new_statement);
                break;
                case ExpressionScaffoldingType::EXPR_WITH_BRACKETS:
                    RulesForExprWithBrackets(new_statement); //Buna bak
                break;
                case ExpressionScaffoldingType::EXPR_WITH_NONE:
                    RulesForExprWithNone(new_statement);
                break;
                default:
                    VERIFY_NOT_REACHED();
                break;

            }

            if (PeekFront(0) == TokenType::SEMICOLON || PeekFront(0) == TokenType::END_OF_STATEMENT) {
                DispatchStatement(context->SendStatement(), context);
                Consume();
            }

            DecideStatementType(new_statement);
            VERIFY(new_statement->Type() != StatementType::INVALID_STATEMENT, "Invalid statement type");


            BuildStatement(new_statement, context); //statement building loop

            RetNode = StatementParser(new_statement, context);
        }

        context->PopCallStack();
        return RetNode;
    }

    ASTNode* Parser::StatementParser(Statement* statement, ParserContext* context) {

            VERIFY(statement, "statement can not be nullptr");

            context->PushCallStack(FUNCTION_NAME());

            NodeBranchInfo* branchInfo = nullptr;

            if (statement->isImmediate()) {
                context->PopCallStack();
                return statement->GenerateASTImmediate(context);
            }

            branchInfo = statement->ParseTokens(context);

            if (context->isAnalyzingLoopDone()) {
                context->PopCallStack();
                return ValidateSemanticsByVisitor(branchInfo);
            }

            context->PopCallStack();
            return branchInfo->Node();

}

    void Parser::DispatchStatement(Statement *statement, ParserContext *context) {
        VERIFY(context->isThereAnyWaitingStatement(), "There is no waiting statement");
        switch (context->TopOfWaitingStack()->Type()) {
            case StatementType::IF_STATEMENT:
                if (!context->TopOfWaitingStack()->ConsequentExpr() &&
                     context->TopOfWaitingStack()->SearchForSpecificHolder(
                         StatementHolderType::CONSEQUENT, statement->ParentID().value())
                         ) {
                    context->TopOfWaitingStack()->SetConsequentExpr(statement);
                    context->PopFromWaitingStack();
                }
                if (!context->TopOfWaitingStack()->AlternateExpr() &&
                    context->TopOfWaitingStack()->SearchForSpecificHolder(
                        StatementHolderType::ALTERNATE, statement->ParentID().value())
                        ) {
                    context->TopOfWaitingStack()->SetAlternateExpr(statement);
                    context->PopFromWaitingStack();
                }
            break;

            case StatementType::RETURN_STATEMENT:
                if (!statement->ConsequentImm()) {
                    auto expr = dynamic_cast<ImmediateStatement*>(statement);
                    VERIFY(expr, "Statement should be ImmediateStatement");
                    if (context->TopOfWaitingStack()->SearchForSpecificHolder(
                            StatementHolderType::CONSEQUENT, statement->ParentID().value())
                            ) {
                        context->TopOfWaitingStack()->SetConsequentImm(expr);
                        context ->PopFromWaitingStack();
                    }
                }
            break;
            default:
                VERIFY_NOT_REACHED();

        }
    }



    void Parser::RulesForIfExpr(Statement *new_statement) {

        int idx = 0;

        StatementHolder statement_holder;
        statement_holder.holder_type = StatementHolderType::CONSEQUENT;

        EndOfStatement end_of_statement;

        new_statement->PushToken(Consume());
        m_CurrentTokenCounter->PushToBracketStack(Consume().Type);

        idx = 0;

        while (!m_CurrentTokenCounter->CheckWhetherClosingTokenComeYet(TokenType::R_BRACKET)) {
            idx++;
            if (PeekFront(0) == TokenType::L_BRACKET) {
                m_CurrentTokenCounter->PushToBracketStack(Consume().Type);
            }
        }

        while (idx-- > 0) {
            new_statement->PushToken(Consume());
        }

        new_statement->PushToken(statement_holder);

        statement_holder.holder_type = StatementHolderType::ALTERNATE;

        if (PeekFront(0) == TokenType::ELSE) {
            new_statement->PushToken(Consume());
            new_statement->PushToken(statement_holder);
        } else {
            VoidToken void_token;
            new_statement->PushToken(void_token);
            new_statement->PushToken(void_token);
        }

        new_statement->PushToken(end_of_statement);

    }
    void Parser::RulesForExprWithBracketsAndBraces(Statement *new_statement) {
        int idx = 0;
        EndOfStatement end_of_statement;

        new_statement->PushToken(Consume());
        m_CurrentTokenCounter->PushToBracketStack(Consume().Type);

        idx = 0;

        while (!m_CurrentTokenCounter->CheckWhetherClosingTokenComeYet(TokenType::R_BRACKET)) {
            idx++;
            if (PeekFront(0) == TokenType::L_BRACKET) {
                m_CurrentTokenCounter->PushToBracketStack(Consume().Type);
            }
        }

        while (idx-- > 0) {
            new_statement->PushToken(Consume());
        }

        m_CurrentTokenCounter->PushToBraceStack(Consume().Type);

        while (!m_CurrentTokenCounter->CheckWhetherClosingTokenComeYet(TokenType::R_BRACE)) {
            idx++;
            if (PeekFront(0) == TokenType::L_BRACE) {
                m_CurrentTokenCounter->PushToBracketStack(Consume().Type);
            }
        }

        while (idx-- > 0) {
            new_statement->PushToken(Consume());
        }

        new_statement->PushToken(end_of_statement);
    }
    void Parser::RulesForExprWithBracketsAndOptionalBraces(Statement *new_statement) {
        int idx = 0;

        new_statement->PushToken(Consume());
        m_CurrentTokenCounter->PushToBracketStack(Consume().Type);

        idx = 0;

        while (!m_CurrentTokenCounter->CheckWhetherClosingTokenComeYet(TokenType::R_BRACKET)) {
            idx++;
            if (PeekFront(0) == TokenType::L_BRACKET) {
                m_CurrentTokenCounter->PushToBracketStack(Consume().Type);
            }
        }

        StatementHolder statement_holder;
        statement_holder.holder_type = StatementHolderType::CONSEQUENT;
        new_statement->PushToken(statement_holder);

        EndOfStatement end_of_statement;
        new_statement->PushToken(end_of_statement);

    }
    void Parser::RulesForExprWithBrackets(Statement* new_statement) {
        int idx = 0;

        new_statement->PushToken(Consume());
        m_CurrentTokenCounter->PushToBracketStack(Consume().Type);

        idx = 0;

        while (!m_CurrentTokenCounter->CheckWhetherClosingTokenComeYet(TokenType::R_BRACKET)) {
            idx++;
            if (PeekFront(0) == TokenType::L_BRACKET) {
                m_CurrentTokenCounter->PushToBracketStack(Consume().Type);
            }
        }

        while (idx-- > 0) {
            new_statement->PushToken(Consume());
        }

        EndOfStatement end_of_statement;
        new_statement->PushToken(end_of_statement);
    }

    void Parser::RulesForExprWithNone(Statement* new_statement) {}


    void Parser::DecideStatementType(Statement *&statement) {
        switch (statement->Tokens().at(0).Type) {
            case TokenType::IF:
                statement = statement->ConvertTo<IfStatement>();
                break;
            case TokenType::L_BRACE:
                statement = statement->ConvertTo<ScopeStatement>();
                break;
            case TokenType::L_BRACKET:
                statement = statement->ConvertTo<BracketStatement>();
                break;
            case TokenType::WHILE:
                statement = statement->ConvertTo<WhileStatement>();
                break;
            case TokenType::FOR:
                statement = statement->ConvertTo<ForStatement>();
                break;
            case TokenType::FUNCTION:
                statement = statement->ConvertTo<FunctionStatement>();
                break;
            case TokenType::RETURN:
                statement = statement->ConvertTo<ReturnStatement>();
                break;
            case TokenType::BINARY_OP:
                statement = statement->ConvertTo<BinaryOpStatement>();
                break;
            case TokenType::UNARY_OP:
                statement = statement->ConvertTo<UnaryOpStatement>();
                break;
            case TokenType::VAR:
                case TokenType::LET:
                statement = statement->ConvertTo<VariableDeclarationStatement>();
                break;

            default:
                VERIFY_NOT_REACHED();
                break;
        }
    }

    void Parser::BuildStatement(Statement *&statement, ParserContext *context) {
        switch (statement->Type()) {
            case StatementType::IF_STATEMENT:
                BuildIf(statement, context);
            case StatementType::FOR_STATEMENT:
                BuildFor(statement, context);
            case StatementType::WHILE_STATEMENT:
                BuildWhile(statement, context);
            case StatementType::FUNCTION_STATEMENT:
                BuildFunction(statement, context);
            case StatementType::RETURN_STATEMENT:
                BuildReturn(statement, context);
                break;
            default:
                VERIFY_NOT_REACHED();
        }
    }


Token &Parser::Consume() {

    auto CurrentToken = tokenCounter()->GetCurrentTokenPtr();
    tokenCounter()->Next();
    if (tokenCounter()->Type() == TokenCounterType::MINOR)
        Context()->incConsumeCount();
    return *CurrentToken;
};
    Token Parser::GetPreviousTokenWithoutGoingBack() {
        return tokenCounter()->GetPrevTokenWithoutGoingBack();
    }



TokenType Parser::PeekFront(int Distance){
        return tokenCounter()->Peek(Distance)->Type;
};

TokenType Parser::PeekHind(int Distance){
    return tokenCounter()->Peek(-Distance)->Type;
}
    Token Parser::DebugPeek(int Distance) {
        return *tokenCounter()->Peek(Distance);
    }

    bool Parser::DecideIfSingleTokenStatement() { //burası multithread olabilir
    if (PeekHind(1) == TokenType::L_BRACKET && PeekFront(1) == TokenType::R_BRACKET)
        return true;
    if (PeekHind(1) == TokenType::L_BRACE && PeekFront(1) == TokenType::R_BRACE)
        return true;
    if (PeekFront(1) == TokenType::COLON || PeekFront(1) == TokenType::COMMA)
        return true;
    if (PeekHind(1) == TokenType::SEMICOLON && PeekFront(1) == TokenType::SEMICOLON)
        return true;
    return false;
}

    void Parser::BuildIf(Statement *&statement, ParserContext *context) {


        auto& tokens = statement->Tokens();

        for (auto token : tokens) {
            if (token.Type == TokenType::STATEMENT_HOLDER) {
                if (token.holder_type == StatementHolderType::CONSEQUENT) {
                    statement->awaitExpression(context);
                }
                if (token.holder_type == StatementHolderType::ALTERNATE) {
                    statement->awaitExpression(context);
                }
            }
        }

        auto focus = statement->FocusOnTestStatement(context);
        auto expr = dynamic_cast<ImmediateStatement*>(focus);

        VERIFY(expr, "Focus cant be nullptr");
        statement->SetTestExpr(expr);

    }

    void Parser::BuildFor(Statement*& statement, ParserContext* context){}
    void Parser::BuildWhile(Statement*& statement, ParserContext* context){}
    void Parser::BuildFunction(Statement*& statement, ParserContext* context){}
    void Parser::BuildReturn(Statement*& statement, ParserContext* context){}
    void Parser::BuildBinaryOp(Statement*& statement, ParserContext* context){}
    void Parser::BuildUnaryOp(Statement*& statement, ParserContext* context){}
    void Parser::BuildVariableDeclaration(Statement*& statement, ParserContext* context){}
    void Parser::BuildScope(Statement*& statement, ParserContext* context){}

ASTNode* Parser::ValidateSemanticsByVisitor(NodeBranchInfo* NodeInfo){

}

}

