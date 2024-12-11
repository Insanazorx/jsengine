#include "Parser.h"
#include "AST.h"
#include "debug.h"

#include <optional>
#include <string>


namespace JSLib {
Parser* Parser::m_Instance = nullptr;

IfStatement* Parser::CreateNewIfStatement() {
    auto NewStatement = IfStatement::Create();

        NewStatement->PushTestTokens(ConsumeSpecificSpan(TokenType::L_PARENTHESES,TokenType::R_PARENTHESES).value());

        NewStatement->PushConseqTokens(ConsumeSpecificSpan(TokenType::L_BRACE,TokenType::R_BRACE).value());

        if (PeekFront(1) == TokenType::ELSE) {
            Consume();
            NewStatement->ApproveAlternateToken(ConsumeSpecificSpan(TokenType::L_BRACE,TokenType::R_BRACE).value());
        }
            return NewStatement;
}
    ForStatement* Parser::CreateNewForStatement() {

    }


        BinaryOpStatement* Parser::CreateNewBinaryOpStatement() {
            auto NewStatement = BinaryOpStatement::Create();

            NewStatement->AddToChain(GetPreviousTokenWithoutGoingBack());


            bool TurnIsOnBinaryOpStatement = true;
            while (PeekFront(0) == TokenType::NUMERIC ||
                PeekFront(0) == TokenType::IDENTIFIER||
                PeekFront(0) == TokenType::BINARY_OP)
            {
                if (PeekFront(0) == TokenType::BINARY_OP && TurnIsOnBinaryOpStatement) {
                    NewStatement->AddToChain(Consume());
                    TurnIsOnBinaryOpStatement = false;
                }
                if (PeekFront(0) == TokenType::IDENTIFIER || PeekFront(0) == TokenType::NUMERIC
                    && TurnIsOnBinaryOpStatement) {
                    NewStatement->AddToChain(Consume());
                    TurnIsOnBinaryOpStatement = true;
                }
            }
            return NewStatement;
        };
        ASTNode* Parser::RecognizeStatementOrRedirectNode(std::optional<std::vector<Token>>& tokens, ParserContext* context) {

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

            ASTNode* RetValue;
            TokenType debugType;
            BinaryOpStatement* binaryOpStatement;

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
                case TokenType::BINARY_OP:
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
                    Consume();
                    break;
                case TokenType::IDENTIFIER:
                case TokenType::NUMERIC:
                    if (DecideIfSingleTokenStatement())
                        VERIFY_NOT_REACHED();
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
                return GeneratePartialAST(branchInfo);
            }

            context->PopCallStack();
            return branchInfo->ExtractDummyNode();

}

std::optional<std::vector<Token>> Parser::ConsumeSpecificSpan(TokenType FirstToken, TokenType LastToken) {

    std::vector<Token> TokenList;

    VERIFY(ExpectToken(FirstToken), "Expected "<< Lexer::StringifyTokenType(FirstToken)<<" type of token after consuming a specific token!");
            VERIFY(ExpectTokenAtAnywhere(LastToken),"Expected " << Lexer::StringifyTokenType(LastToken)<< " at anywhere in source code!");

            Consume();
            int LoopCounter = 0;
    while (PeekFront(0) != LastToken) {
        VERIFY(LoopCounter<20, "LoopCounter has been exceeded");
        DEBUG(LOGGER_BANNER(PARSER)<<"Pushed with Token Type: " << Lexer::StringifyTokenType(*tokenCounter()->GetCurrentTokenPtr()));
        TokenList.push_back(Consume());
    }
            Consume();

    return std::move(*new std::optional(TokenList));
};
std::optional<std::vector<Token>> Parser::ConsumeStatement(StatementType statementType) {

    Token* CurrentToken;

    if (tokenCounter()->GetCurrentTokenPtr()->StatType != statementType)
        return std::nullopt;

    auto StatementList = *new std::vector<Token>;

    while (tokenCounter()->GetCurrentTokenPtr()->Type != TokenType::END_OF_SCOPE) {
        if (tokenCounter()->GetCurrentTokenPtr()->StatType != StatementType::INVALID_STATEMENT) {
            CurrentToken = tokenCounter()->GetCurrentTokenPtr();
            tokenCounter()->PushToStatementTracerStack();
            tokenCounter()->Next();
        }

        StatementList.push_back(*CurrentToken);

        if (tokenCounter()->GetCurrentTokenPtr()->Type == TokenType::END_OF_STATEMENT) {
            tokenCounter()->PopFromStatementTracerStack();
            break;
        }
    }

    return std::move(*new std::optional<std::vector<Token>>(StatementList));
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

TokenType Parser::PeekFront(int Distance){
        return tokenCounter()->Peek(Distance)->Type;
};
TokenType Parser::PeekHind(int Distance){
    return tokenCounter()->Peek(-Distance)->Type;
}
    Token Parser::DebugPeek(int Distance) {
        return *tokenCounter()->Peek(Distance);
    }

bool Parser::ExpectToken (TokenType TokenTypeToExpect) {
    if (PeekFront(0) == TokenTypeToExpect)
        return true;
    return false;
    };

bool Parser::ExpectTokenAtAnywhere (TokenType TokenTypeToExpect) {
    int pos = 0;
    while (PeekFront(pos) != TokenType::END_OF_STREAM)
        if (PeekFront(pos++) == TokenTypeToExpect)
            return true;
    return false;
}

    bool Parser::DecideIfSingleTokenStatement() { //burası multithread olabilir
    if (PeekHind(1) == TokenType::L_PARENTHESES && PeekFront(1) == TokenType::R_PARENTHESES)
        return true;
    if (PeekHind(1) == TokenType::L_BRACE && PeekFront(1) == TokenType::R_BRACE)
        return true;
    if (PeekFront(1) == TokenType::SEMICOLON || PeekFront(1) == TokenType::COMMA)
        return true;
    if (PeekHind(1) == TokenType::END_OF_STATEMENT && PeekFront(1) == TokenType::END_OF_STATEMENT)
        return true;
    return false;
}


ASTNode* Parser::GeneratePartialAST(NodeBranchInfo* NodeInfo){

    auto ScaffoldingTopLevelNode = Builder()->GenerateScaffoldingByShape();
    Builder()->visitor()->SetCurrentToTopLevelNode(ScaffoldingTopLevelNode);

    while (Builder()->visitor()->SwitchToOtherChild()) {
        while (Builder()->visitor()->CheckIfThereAreChildren()) {
            if(Builder()->visitor()->CheckIfMoreThanOneChild())
                Builder()->visitor()->PushToNodeStack(
                    Builder()->visitor()->CurrentNode()
                );
            Builder()->visitor()->PlaceNodeAndIterateToChild(
                Builder()->SearchSpecificNode(NodeInfo, Builder()->visitor()->CurrentNode())
            );
        }
        if (Builder()->visitor()->CheckIfStackContainsElement())
            Builder()->visitor()->ReturnToLastStackElement();
    }
    return Builder()->visitor()->CurrentNode();
}



ASTNode* GenerateAST(ParserContext* context) {

}

}

