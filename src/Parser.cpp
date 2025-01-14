#include "Parser.h"
#include "AST.h"
#include "debug.h"

#include <optional>
#include <string>


namespace JSLib {
Parser* Parser::m_Instance = nullptr;

IfStatement* Parser::CreateNewIfStatement() {
    auto NewStatement = IfStatement::Create();
    auto PermanentConditionCallableForParentheses = [&](TokenType type = TokenType::L_PARENTHESES)->int {
        tokenCounter()->PushToBracketOrBraceStack(type);
        Consume();
        int i = 0;
        while (PeekFront(i) != TokenType::END_OF_STREAM) {
            DEBUG(LOGGER_BANNER(PARSER) << "PeekFront(i): " << Lexer::StringifyTokenType(PeekFront(i)));
            //if ( ( a - b ) * c ) { return 1 + 2 + d ; }")
            if (PeekFront(i) == type) {
                tokenCounter()->PushToBracketOrBraceStack(type);
            } else if (tokenCounter()->CheckIfRightBracketOrBraceProperAndPairThem(PeekFront(i))) {
                return i;
            }
            i++;
        }
        return -1;
    };
    auto PermanentConditionCallableForBraces = [&](TokenType type = TokenType::L_BRACE)->int {
        if (PeekFront(0)==TokenType::R_PARENTHESES)
            Consume();

        tokenCounter()->PushToBracketOrBraceStack(type);

        Consume();

        int i = 0;

        while (PeekFront(i++) != TokenType::END_OF_STREAM) {
            if (PeekFront(i) == type) {
                tokenCounter()->PushToBracketOrBraceStack(type);
            }
            if (tokenCounter()->CheckIfRightBracketOrBraceProperAndPairThem(PeekFront(i))) {
                return i;
            }
        }
        VERIFY_NOT_REACHED();
    };
    NewStatement->PushTestTokens(ConsumeSpecificSpan(
    [&]()->void {
        ExpectToken(TokenType::L_PARENTHESES);
        ExpectTokenAtAnywhere(TokenType::R_PARENTHESES);
    }, PermanentConditionCallableForParentheses
    ).value());

        NewStatement->PushConseqTokens(ConsumeSpecificSpan([&]()->void {
            ExpectToken(TokenType::L_BRACE);
            ExpectTokenAtAnywhere(TokenType::R_BRACE);
        },PermanentConditionCallableForBraces
        ).value());

        if (PeekFront(1) == TokenType::ELSE) {
            Consume();
            NewStatement->ApproveAlternateToken(ConsumeSpecificSpan([&]()->void {
                ExpectToken(TokenType::L_BRACE);
                ExpectTokenAtAnywhere(TokenType::R_BRACE);
            },PermanentConditionCallableForBraces).value());
        }
            return NewStatement;
}
    WhileStatement* Parser::CreateNewWhileStatement() {

    }


    BinaryOpStatement* Parser::CreateNewBinaryOpStatement() {
    auto NewStatement = BinaryOpStatement::Create();

    if (!(PeekFront(0) == TokenType::L_PARENTHESES))
        NewStatement->AddToChain(GetPreviousTokenWithoutGoingBack());



    while (PeekFront(0) == TokenType::NUMERIC       ||
           PeekFront(0) == TokenType::IDENTIFIER    ||
           PeekFront(0) == TokenType::BINARY_OP     ||
           PeekFront(0) == TokenType::L_PARENTHESES ||
           PeekFront(0) == TokenType::R_PARENTHESES)
            {

                if (PeekFront(0) == TokenType::L_PARENTHESES) {
                    tokenCounter()->PushToBracketOrBraceStack(TokenType::L_PARENTHESES);
                    NewStatement->AddToChain(Consume());
                    if (PeekHind(1)==TokenType::NUMERIC)
                        VERIFY_NOT_REACHED(); //TODO: throw exception via SemanticError object
                    if (PeekFront(1) == TokenType::BINARY_OP     &&
                        !(tokenCounter()->Peek(1)->Lexeme == "+" ||
                        tokenCounter()->Peek(1)->Lexeme == "-" ))
                        VERIFY_NOT_REACHED(); //TODO: throw exception via SemanticError object
                    if (tokenCounter()->Peek(1)->Lexeme == "+" ||
                        tokenCounter()->Peek(1)->Lexeme == "-" )
                        tokenCounter()->Peek(1)->isSign = true;

                    continue;
                }

                if (PeekFront(0) == TokenType::R_PARENTHESES) {
                    if (!tokenCounter()->CheckIfRightBracketOrBraceProperAndPairThem(TokenType::R_PARENTHESES))
                        VERIFY_NOT_REACHED(); //TODO: throw exception via SemanticError object
                    if (!(PeekFront(0) == TokenType::BINARY_OP || PeekFront(0) == TokenType::R_PARENTHESES))
                        VERIFY_NOT_REACHED(); //TODO: throw exception via SemanticError object
                    if (!(PeekHind(1) == TokenType::IDENTIFIER || PeekHind(1) == TokenType::NUMERIC))
                        VERIFY_NOT_REACHED(); //TODO: throw exception via SemanticError object
                    NewStatement->AddToChain(Consume());
                    continue;
                }

                if (PeekFront(0) == TokenType::BINARY_OP) {
                    NewStatement->AddToChain(Consume());
                    if ((tokenCounter()->Peek(0)->Lexeme == "+" ||
                        tokenCounter()->Peek(0)->Lexeme == "-") &&
                        (tokenCounter()->Peek(1)->Type == TokenType::NUMERIC ||
                        tokenCounter()->Peek(1)->Type == TokenType::IDENTIFIER)) {
                        tokenCounter()->Peek(0)->isSign = true;
                        NewStatement->AddToChain(Consume());
                        continue;
                    }
                }
                if ((PeekFront(0) == TokenType::IDENTIFIER || PeekFront(0) == TokenType::NUMERIC)) {
                    NewStatement->AddToChain(Consume());
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
                case TokenType::L_PARENTHESES:
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


template <CallableReturnsInt T, CallableReturnsVoid F>
std::optional<std::vector<Token>> Parser::ConsumeSpecificSpan(F&& StartingCondition, T&& PermanentCondition) {

            std::vector<Token> TokenList;
            StartingCondition();
            int i = PermanentCondition();

            while (i-- > 0)
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

