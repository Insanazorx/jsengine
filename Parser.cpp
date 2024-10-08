#include "Parser.h"
#include "AST.h"
#include "StdLib.h"
using namespace Std;



namespace JSLib {
        
IfStatement* Parser::CreateNewIfStatement() {
    auto NewStatement = new IfStatement();
        NewStatement
            ->PushTestTokens(
                (ConsumeSpecificSpan(
                    (ExpectToken(TokenType::LPARENTHESS).hasValue() ? 
                        ExpectToken(TokenType::LPARENTHESS).get() : TokenType::INVALID), 
                    (ExpectToken(TokenType::RPARENTHESS).hasValue() ? 
                        ExpectToken(TokenType::RPARENTHESS).get() : TokenType::INVALID) 
                    ).hasValue() ? 
                    ConsumeSpecificSpan(
                        ExpectToken(TokenType::LPARENTHESS).get(),
                        ExpectToken(TokenType::RPARENTHESS).get()
                    )
                    : NullTokenList).get()
                    )
                ->PushConseqTokens(
                    (ConsumeSpecificSpan(
                        (ExpectToken(TokenType::LBRACKET).hasValue() ? 
                            ExpectToken(TokenType::LBRACKET).get() : TokenType::INVALID), 
                        (ExpectToken(TokenType::RBRACKET).hasValue() ? 
                            ExpectToken(TokenType::RBRACKET).get() : TokenType::INVALID) 
                    ).hasValue() ? 
                    ConsumeSpecificSpan(
                        ExpectToken(TokenType::LBRACKET).get(), 
                        ExpectToken(TokenType::RBRACKET).get())
                        : NullTokenList).get()
                );
            if (PeekFront(1).get() == TokenType::ELSE)
                NewStatement
                    ->ApproveAlternateToken(
                        (ConsumeSpecificSpan(
                            (ExpectToken(TokenType::LBRACKET).hasValue() ? 
                                ExpectToken(TokenType::LBRACKET).get() : TokenType::INVALID), 
                            (ExpectToken(TokenType::RBRACKET).hasValue() ? 
                                ExpectToken(TokenType::RBRACKET).get() : TokenType::INVALID) 
                            ).hasValue() ? ConsumeSpecificSpan(
                                ExpectToken(TokenType::LBRACKET).get(), 
                                ExpectToken(TokenType::RBRACKET).get())
                                : NullTokenList 
                            ).get()
                        );
            return NewStatement;
        }
        ForStatement* Parser::CreateNewForStatement() {
            auto NewStatement = new ForStatement();
            NewStatement
                ->PushInitialConditionTokens(ConsumeStatement(ExpectStatement(StatementType::VARIABLE_DECLARATION).hasValue() ? ExpectStatement(StatementType::VARIABLE_DECLARATION).get() : StatementType::INVALID_STATEMENT).get())
                ->PushTestTokens(ConsumeStatement(ExpectStatement(StatementType::BOOL_STATEMENT).hasValue() ? ExpectStatement(StatementType::BOOL_STATEMENT).get() : StatementType::INVALID_STATEMENT).get())
                ->PushIterationTokens(ConsumeStatement(ExpectStatement(StatementType::ANY_OF_STATEMENT_TYPES).hasValue() ? ExpectStatement(StatementType::ANY_OF_STATEMENT_TYPES).get() : StatementType::INVALID_STATEMENT).get())
                ->PushConseqTokens(
                    (ConsumeSpecificSpan(
                        (ExpectToken(TokenType::LBRACKET).hasValue() ? 
                            ExpectToken(TokenType::LBRACKET).get() : TokenType::INVALID), 
                        (ExpectToken(TokenType::RBRACKET).hasValue() ? 
                            ExpectToken(TokenType::RBRACKET).get() : TokenType::INVALID) 
                        ).hasValue() ? 
                        ConsumeSpecificSpan(
                            ExpectToken(TokenType::LBRACKET).get(), 
                            ExpectToken(TokenType::RBRACKET).get())
                                    : NullTokenList).get()
                    );    
            return NewStatement;
        } 
        BinaryOpStatement* Parser::CreateNewBinaryOpStatement() {
            auto NewStatement = new BinaryOpStatement();
            while ( PeekFront(1).get() == TokenType::BINARY_OP || PeekFront(1).get() == TokenType::NUMERIC)
                NewStatement->AddToChain(Consume());
        
        return NewStatement;
    };
void Parser::Analyze() {
        
        encapsulator().InitScope(); 
        
        while (PeekFront(1).hasValue()) {
            switch (Consume()->Type) {
                case TokenType::IF:
                    StatementParser(Move(*CreateNewIfStatement()));
                    break;
                case TokenType::BINARY_OP:
                    StatementParser(Move(*CreateNewBinaryOpStatement()));
                    break;
                case TokenType::CLASS:
                    //StatementParser(Move(*new Statement));
                    break;
                case TokenType::FOR:
                    StatementParser(Move(*CreateNewForStatement()));
                    break;
                case TokenType::WHILE:
                    //StatementParser(Move(*new Statement));
                    break;
                case TokenType::FUNCTION:
                    //StatementParser(Move(*new Statement));
                    break;
                case TokenType::LET:
                case TokenType::VAR:
                    //StatementParser(Move(*new Statement));
                    break;
                default:
                    break;
            }
        }
    }

void Parser::StatementParser(Statement&& statement) {
    auto Info = statement.ParseTokens();
    
    if (statement.Type() == StatementType::BINARY_OP_STATEMENT)
        encapsulator().EncapsulateWithScope(Info);
    
    auto RawNodePtr = GeneratePartialAST(Info);
    
    ASTcx().NodeStack().push_back(RawNodePtr);
    ASTcx().UpdateContext(Move(Info));
}

Optional<List<Token*>> Parser::ConsumeSpecificSpan(TokenType FirstToken, TokenType LastToken) {
    auto TokenList = *new List<Token*>;
    
    if (tokenCounter().GetCurrentTokenPtr()->Type != FirstToken) 
        return new Optional<List<Token*>>(NullTokenList);
    
    while (tokenCounter().GetCurrentTokenPtr()->Type != LastToken) {
        TokenList.push_back(tokenCounter().GetCurrentTokenPtr());
        tokenCounter().Next();
    }
    
    TokenList.push_back(tokenCounter().GetCurrentTokenPtr());
    tokenCounter().Next();
    
    return *new Optional<List<Token*>>(TokenList); 
};
Optional<List<Token*>> Parser::ConsumeStatement(StatementType statementType) {
    Token* CurrentToken;
    if (tokenCounter().GetCurrentTokenPtr()->StatType.get() != statementType)
        return NullTokenList;
    auto StatementList = *new List<Token*>;
    while (tokenCounter().GetCurrentTokenPtr()->Type != TokenType::END_OF_SCOPE) {
        if (tokenCounter().GetCurrentTokenPtr()->StatType.hasValue()) {
            CurrentToken = tokenCounter().GetCurrentTokenPtr();
            tokenCounter().PushToStatementTracerStack();
            tokenCounter().Next();
        }
        StatementList.push_back(CurrentToken);
        
        if (tokenCounter().GetCurrentTokenPtr()->Type == TokenType::END_OF_STATEMENT) {
            tokenCounter().PopFromStatementTracerStack();
            break;
        } 
        }
    return *new Optional<List<Token*>>(StatementList);

};


Token* Parser::Consume() {
    auto CurrentToken = tokenCounter().GetCurrentTokenPtr();
    tokenCounter().Next();
    return CurrentToken;
};
Optional<TokenType> Parser::PeekFront(int Distance){
    return tokenCounter().Peek(Distance);
};
Optional<TokenType> Parser::PeekHind(int Distance){
    return tokenCounter().Peek(-Distance);
};
Optional<TokenType> Parser::ExpectToken(TokenType tokenType){
    auto CurrentToken = tokenCounter().GetCurrentTokenPtr();
    while (CurrentToken->Type != tokenType) 
        CurrentToken = tokenCounter().Next();
    return CurrentToken;
};
Optional<StatementType> Parser::ExpectStatement(StatementType statType){
    auto CurrentToken = tokenCounter().GetCurrentTokenPtr();
    while (CurrentToken->StatType.get() != statType) 
        CurrentToken = tokenCounter().Next();
    return statType;
};

ASTNode* Parser::GeneratePartialAST(NodeBranchInfo& NodeInfo){
    auto& Builder = ASTcx().Builder();
    Builder.shapeBuilder().SetInfo(NodeInfo);
    Builder.shapeBuilder().SetMappings();
    Builder.SetBuilderParametersByLookingShape();
    auto ScaffoldingTopLevelNode = Builder.GenerateScaffoldingByShape();
    Builder.visitor().SetCurrentToTopLevelNode(ScaffoldingTopLevelNode);   
    
    while (Builder.visitor().SwitchToOtherChild()) {
        while (Builder.visitor().CheckIfThereAreChildren()) {
            if(Builder.visitor().CheckIfMoreThanOneChild())
                Builder.visitor().PushToNodeStack(
                    Builder.visitor().CurrentNode()
                );
            Builder.visitor().PlaceNodeAndIterateToChild(
                Builder.SearchSpecificNode(NodeInfo, Builder.visitor().CurrentNode())
            ); 
        }
        if (Builder.visitor().CheckIfStackContainsElement())
            Builder.visitor().ReturnToLastStackElement();
    }  
}

ASTNode* GenerateAST(ASTContext& cx) {

}

}