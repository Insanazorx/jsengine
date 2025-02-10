#include "Parser.h"
#include "AST.h"
#include "debug.h"

#include <optional>
#include <string>


namespace JSLib {
Parser* Parser::m_Instance = nullptr;

    ReturnStatement* Parser::CreateNewReturnStatement() {
        auto NewStatement = ReturnStatement::Create();
        Consume();
        NewStatement->awaitExpression(Context());
        return NewStatement;
    }

IfStatement* Parser::CreateNewIfStatement() {
    auto NewStatement = IfStatement::Create();
        Context()->SyntacticValidator()->Expect(TokenType::L_BRACKET);
    return NewStatement;
}
    WhileStatement* Parser::CreateNewWhileStatement() {

    }




    BinaryOpStatement* Parser::CreateNewBinaryOpStatement() {
        auto NewStatement = BinaryOpStatement::Create();
            return NewStatement;
        };
        ASTNode* Parser::RecognizeStatementOrRedirectNode(ErrorOr<std::vector<Token>> tokens, ParserContext* context) {

            context->PushCallStack(FUNCTION_NAME());

            VERIFY_ONCE(!tokens.value().empty(),"Tokens should not provided at first");

            if (!tokens.value().empty()) {
                m_CurrentTokenCounter = m_MajorTokenCounter;
            } else {
                m_MinorTokenCounter = MinorTokenCounter::Create();
                m_CurrentTokenCounter = m_MinorTokenCounter;
                m_CurrentTokenCounter->SetTokens(tokens.value());
            }

            VERIFY_ONCE(m_MajorTokenCounter->Type() == TokenCounterType::MAJOR, "MajorTokenCounter should be selected first");

            ASTNode* RetValue;
            TokenType debugType;

        while (PeekFront(0) != TokenType::END_OF_STREAM) {

            if (tokenCounter()->Type() == TokenCounterType::MAJOR)
                for (;Context()->ConsumeCountFromMinorCounters() > 0; Context()->decConsumeCount())
                    Consume();
            debugType = PeekFront(0);
            switch (debugType) {
                case TokenType::IF:
                    Consume();
                    RetValue = StatementParser(CreateNewIfStatement(), context);
                    break;
                case TokenType::L_BRACE:

                case TokenType::L_BRACKET:
                    case TokenType::BINARY_OP:
                    if (Context()->isThereAnyWaitingStatement()) {
                        Context()->PushSendingStack(CreateNewBinaryOpStatement());
                        continue;
                    }
                    RetValue = StatementParser(CreateNewBinaryOpStatement(), context);
                    break;
                case TokenType::CLASS:
                    //StatementParser(std::move(*new Statement));
                    break;
                case TokenType::FOR:
                    //RetValue = StatementParser(CreateNewForStatement(), context);
                    break;
                case TokenType::WHILE:
                    //StatementParser(std::move(*new Statement));
                    break;
                case TokenType::FUNCTION:
                    //StatementParser(std::move(*new Statement));
                break;
                case TokenType::RETURN:
                    CreateNewReturnStatement();
                    break;
                case TokenType::IDENTIFIER:
                case TokenType::NUMERIC:
                    if (DecideIfSingleTokenStatement()) {
                        auto NewStatement = SingleTokenStatement::Create(Consume());
                        DispatchWaitingStatement(Context()->RedirectWaitingStatement(),
                                                NewStatement);
                    }
                    Consume();
                    break;
                case TokenType::R_BRACKET:
                case TokenType::SEMICOLON:
                    if (Context()->isThereAnyWaitingStatement()) {
                        auto WaitingStatement = Context()->RedirectWaitingStatement();
                        DispatchWaitingStatement(WaitingStatement,
                                                Context()->SendStatement());
                        RetValue = StatementParser(WaitingStatement, context);
                    }

                    Consume();
                    break;
                default:
                    DEBUG(LOGGER_BANNER(PARSER)<<"Statement could not be recognized, lexeme: " << tokenCounter()->GetPrevTokenWithoutGoingBack().Lexeme);
                    VERIFY_NOT_REACHED();
                    break;

            }
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
                return ValidateSyntaxByVisitor(branchInfo);
            }

            context->PopCallStack();
            return branchInfo->Node();

}


template <Callable F>
std::optional<std::vector<Token>> Parser::ConsumeSpecificSpan(F&& IndexOfLastTokenToConsumeCallable, ParserContext *context) {

            auto TokenList = *new std::vector<Token>();

            TRY_FOR_VOID(Context()->SyntacticValidator()->Expect(TokenType::L_BRACKET));

            auto idx = TRY(IndexOfLastTokenToConsumeCallable());

            while (idx-- > 0)
                TokenList.push_back(Consume());

            return std::move(*new std::optional(TokenList));
};



Token Parser::Consume() {

    auto CurrentToken = tokenCounter()->GetCurrentTokenPtr();
    tokenCounter()->Next();
    if (tokenCounter()->Type() == TokenCounterType::MINOR)
        Context()->incConsumeCount();
    //DEBUG(LOGGER_BANNER(PARSER) << "consumed " << Lexer::StringifyTokenType(*CurrentToken));
    return *CurrentToken;
};
    Token Parser::GetPreviousTokenWithoutGoingBack() {
        return tokenCounter()->GetPrevTokenWithoutGoingBack();
    }

    void Parser::SanitizeEmptyTokens(std::vector<Token>& TokensToSanitize) {
        int i = 0;
        for (const auto& token: TokensToSanitize) {
            if (token.Lexeme.empty())
                TokensToSanitize.erase(TokensToSanitize.begin() + i);
            i++;
        }

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


ASTNode* Parser::ValidateSyntaxByVisitor(NodeBranchInfo* NodeInfo){

}

}

