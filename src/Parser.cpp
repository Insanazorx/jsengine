#include "Parser.h"
#include "AST.h"
#include "debug.h"

#include <optional>
#include <string>


namespace JSLib {
Parser* Parser::m_Instance = nullptr;

    ASTNode* Parser::AnalyzeImpl(std::optional<std::vector<Token>> tokens, ParserContext* context) {

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

        ASTNode* RetValue = nullptr;

        if (tokenCounter()->Type() == TokenCounterType::MAJOR)
            for (;Context()->ConsumeCountFromMinorCounters() > 0; Context()->decConsumeCount()); //consume all minor tokens consumed already

        while (PeekFront(0) != TokenType::END_OF_STREAM) {
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



            if (PeekFront(0) == TokenType::SEMICOLON) {
                Consume();
            }

            BuildAndSpecializeStatement(new_statement, context);
            VERIFY(new_statement->Type() != StatementType::INVALID_STATEMENT, "Invalid statement type");

            int awaiting_statement_count = new_statement->AwaitingExpressionCount();



            RetValue = StatementParser(new_statement, context);
        }

        context->PopCallStack();
        return RetValue;
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


    void Parser::RulesForIfExpr(Statement *new_statement) {

        int idx = 0;
        StatementHolder statement_holder;

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

        if (PeekFront(0) == TokenType::ELSE) {
            new_statement->PushToken(Consume());
            new_statement->PushToken(statement_holder);
        }

    }
    void Parser::RulesForExprWithBracketsAndBraces(Statement *new_statement) {
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
        statement_holder.allowed_types = StatementType::ANY_OF_STATEMENT_TYPES;

        new_statement->PushToken(statement_holder);

    }
    void Parser::RulesForExprWithBrackets(Statement *new_statement) {

    }

    void Parser::RulesForExprWithNone(Statement *new_statement) {}


    void Parser::BuildAndSpecializeStatement(Statement* statement, ParserContext* context) {

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


ASTNode* Parser::ValidateSemanticsByVisitor(NodeBranchInfo* NodeInfo){

}

}

