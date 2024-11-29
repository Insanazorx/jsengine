#include "Parser.h"
#include "AST.h"
#include "debug.h"
#include "StdLib.h"
#include <optional>
#include <string>



namespace JSLib {
Parser* Parser::m_Instance = nullptr;

IfStatement* Parser::CreateNewIfStatement() {
    auto NewStatement = IfStatement::Create();
        NewStatement
            ->PushTestTokens(
                (ConsumeSpecificSpan(
                    (ExpectToken(TokenType::L_PARENTHESES).value_or(TokenType::INVALID)),
                    (ExpectToken(TokenType::R_PARENTHESES).value_or(TokenType::INVALID))
                    ).has_value() ?
                    ConsumeSpecificSpan(
                        ExpectToken(TokenType::L_PARENTHESES).get(),
                        ExpectToken(TokenType::R_PARENTHESES).get()
                    )
                    : std::nullopt).get()
                    )
                ->PushConseqTokens(
                    (ConsumeSpecificSpan(
                        (ExpectToken(TokenType::L_BRACE).has_value() ?
                            ExpectToken(TokenType::L_BRACE).get() : TokenType::INVALID),
                        (ExpectToken(TokenType::R_BRACE).has_value() ?
                            ExpectToken(TokenType::R_BRACE).get() : TokenType::INVALID)
                    ).has_value() ?
                    ConsumeSpecificSpan(
                        ExpectToken(TokenType::L_BRACE).get(),
                        ExpectToken(TokenType::R_BRACE).get())
                        : std::nullopt).get()
                );
            if (PeekFront(1).get() == TokenType::ELSE)
                NewStatement
                    ->ApproveAlternateToken(
                        (ConsumeSpecificSpan(
                            (ExpectToken(TokenType::L_BRACE).has_value() ?
                                ExpectToken(
                                    TokenType::L_BRACE).get() : TokenType::INVALID
                                ),
                                (ExpectToken(TokenType::R_BRACE).has_value() ?
                                    ExpectToken(TokenType::R_BRACE).get() : TokenType::INVALID)
                                ).has_value() ? ConsumeSpecificSpan(
                                    ExpectToken(TokenType::L_BRACE).get(),
                                    ExpectToken(TokenType::R_BRACE).get())
                                : std::nullopt
                            ).get()
                        );
            return NewStatement;
        }
        ForStatement* Parser::CreateNewForStatement() {
            auto NewStatement = ForStatement::Create();
            NewStatement
                ->PushInitialConditionTokens(
                    ConsumeStatement(
                        ExpectStatement(
                            StatementType::VARIABLE_DECLARATION_STATEMENT).has_value() ?
                            ExpectStatement(StatementType::VARIABLE_DECLARATION_STATEMENT).get() : StatementType::INVALID_STATEMENT).get())

                ->PushTestTokens(
                    ConsumeStatement(
                        ExpectStatement(
                            StatementType::BOOL_STATEMENT).has_value() ?
                            ExpectStatement(StatementType::BOOL_STATEMENT).get() : StatementType::INVALID_STATEMENT).get())

                ->PushIterationTokens(
                    ConsumeStatement(
                        ExpectStatement(
                            StatementType::ANY_OF_STATEMENT_TYPES).has_value() ?
                            ExpectStatement(StatementType::ANY_OF_STATEMENT_TYPES).get() : StatementType::INVALID_STATEMENT).get())

                ->PushConseqTokens(
                    (ConsumeSpecificSpan(
                        (ExpectToken(TokenType::L_BRACE).has_value() ?
                                ExpectToken(TokenType::L_BRACE).get() : TokenType::INVALID),
                        (ExpectToken(TokenType::R_BRACE).has_value() ?
                                ExpectToken(TokenType::R_BRACE).get() : TokenType::INVALID)
                                ).has_value() ?
                        ConsumeSpecificSpan(
                            ExpectToken(TokenType::L_BRACE).get(),
                            ExpectToken(TokenType::R_BRACE).get())
                                    : std::nullopt).get()
                    );
            return NewStatement;
        }
        ImmediateStatement* Parser::CreateNewBinaryOpStatement() {
            auto NewStatement = BinaryOpStatement::Create();

            if (PeekHind(1).get() == TokenType::NUMERIC || PeekHind(1).get() == TokenType::ANY_NAME)
                NewStatement->AddToChain(GetPreviousTokenWithoutGoingBack());

            bool TurnIsOnBinaryOpStatement = true;
            while (PeekFront(1).get() == TokenType::NUMERIC ||
                PeekFront(1).get() == TokenType::ANY_NAME||
                PeekFront(1).get() == TokenType::BINARY_OP)
            {
                if (PeekFront(1).get() == TokenType::BINARY_OP && TurnIsOnBinaryOpStatement) {
                    NewStatement->AddToChain(Consume());
                    TurnIsOnBinaryOpStatement = false;
                }
                if (PeekFront(1).get() == TokenType::ANY_NAME ||
                    PeekFront(1).get() == TokenType::NUMERIC && TurnIsOnBinaryOpStatement) {
                    NewStatement->AddToChain(Consume());
                    TurnIsOnBinaryOpStatement = true;
                }
            }
            return NewStatement;
        };
        Node* Parser::RecognizeStatementOrRedirectNode(std::optional<std::list<Token*>>& tokens, ParserContext* context) {

            context->PushCallStack(FUNCTION_NAME());

        if (!tokens.has_value()) {
            m_CurrentTokenCounter = m_MajorTokenCounter;
        }
        else {
            m_CurrentTokenCounter = m_MinorTokenCounter;
            m_CurrentTokenCounter->SetTokens(tokens.get());
        }
        
        Node* RetValue;

        
        while (PeekFront(1).has_value()) {
            switch (Consume()->Type) {
                case TokenType::IF:
                RetValue = StatementParser(CreateNewIfStatement(), context);
                    break;
                case TokenType::BINARY_OP:
                    RetValue = StatementParser(CreateNewBinaryOpStatement(), context);
                    break;
                case TokenType::CLASS:
                    //StatementParser(std::move(*new Statement));
                    break;
                case TokenType::FOR:
                    RetValue = StatementParser(CreateNewForStatement(), context);
                    break;
                case TokenType::WHILE:
                    //StatementParser(std::move(*new Statement));
                    break;
                case TokenType::FUNCTION:
                    //StatementParser(std::move(*new Statement));
                    break;
                default:
                    break;

            }
        }
            context->PopCallStack();
        return RetValue;
    }

    Node* Parser::StatementParser(Statement* statement, ParserContext* context) {

            VERIFY(statement, "statement can not be nullptr");

            context->PushCallStack(new std::string(FUNCTION_NAME()));

            NodeBranchInfo* branchInfo = nullptr;

            if (statement->Type() == StatementType::IMMEDIATE_STATEMENT) {
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

std::optional<std::list<Token*>> Parser::ConsumeSpecificSpan(TokenType FirstToken, TokenType LastToken) {

    auto TokenList = *new std::list<Token*>;
    
    if (tokenCounter()->GetCurrentTokenPtr()->Type != FirstToken) 
        return std::nullopt;
    
    while (tokenCounter()->GetCurrentTokenPtr()->Type != LastToken) {
        TokenList.push_back(tokenCounter()->GetCurrentTokenPtr());
        tokenCounter()->Next();
    }
    
    TokenList.push_back(tokenCounter()->GetCurrentTokenPtr());
    tokenCounter()->Next();
    
    return std::move(*new std::optional<std::list<Token*>>(TokenList));
};
std::optional<std::list<Token*>> Parser::ConsumeStatement(StatementType statementType) {

    Token* CurrentToken;

    if (tokenCounter()->GetCurrentTokenPtr()->StatType != statementType)
        return nullopt;

    auto Statementstd::list = *new std::list<Token*>;

    while (tokenCounter()->GetCurrentTokenPtr()->Type != TokenType::END_OF_SCOPE) {
        if (tokenCounter()->GetCurrentTokenPtr()->StatType != StatementType::INVALID_STATEMENT) {
            CurrentToken = tokenCounter()->GetCurrentTokenPtr();
            tokenCounter()->PushToStatementTracerStack();
            tokenCounter()->Next();
        }

        Statementstd::list.push_back(CurrentToken);
        
        if (tokenCounter()->GetCurrentTokenPtr()->Type == TokenType::END_OF_STATEMENT) {
            tokenCounter()->PopFromStatementTracerStack();
            break;
        }
    }

    return std::move(*new std::optional<std::list<Token*>>(Statementstd::list));
};


Token* Parser::Consume() {
    auto CurrentToken = tokenCounter()->GetCurrentTokenPtr();
    tokenCounter()->Next();
    return CurrentToken;
};
    Token* Parser::GetPreviousTokenWithoutGoingBack() {
        return tokenCounter()->GetPrevTokenWithoutGoingBack();
    }

std::optional<TokenType> Parser::PeekFront(int Distance){
    return tokenCounter()->Peek(Distance)->Type;
};
std::optional<TokenType> Parser::PeekHind(int Distance){
    return tokenCounter()->Peek(-Distance)->Type;
};
std::optional<TokenType> Parser::ExpectToken(TokenType tokenType){
    auto CurrentToken = tokenCounter()->GetCurrentTokenPtr();
    while (CurrentToken->Type != tokenType) 
        CurrentToken = tokenCounter()->Next();
    return CurrentToken->Type;
};
std::optional<StatementType> Parser::ExpectStatement(StatementType statType){
    auto CurrentToken = tokenCounter()->GetCurrentTokenPtr();
    while (CurrentToken->StatType != statType)
        CurrentToken = tokenCounter()->Next();
    return std::move(statType);
};

ASTNode* Parser::GeneratePartialAST(NodeBranchInfo* NodeInfo){

    auto ScaffoldingTopLevelNode = Builder()->GenerateScaffoldingByShape();
    Builder()->visitor().SetCurrentToTopLevelNode(ScaffoldingTopLevelNode);
    
    while (Builder()->visitor().SwitchToOtherChild()) {
        while (Builder()->visitor().CheckIfThereAreChildren()) {
            if(Builder()->visitor().CheckIfMoreThanOneChild())
                Builder()->visitor().PushToNodeStack(
                    Builder()->visitor().CurrentNode()
                );
            Builder()->visitor().PlaceNodeAndIterateToChild(
                Builder()->SearchSpecificNode(NodeInfo, Builder()->visitor().CurrentNode())
            ); 
        }
        if (Builder()->visitor().CheckIfStackContainsElement())
            Builder()->visitor().ReturnToLastStackElement();
    }
    return Builder()->visitor().CurrentNode();
}



ASTNode* GenerateAST(ParserContext* context) {

}

}
