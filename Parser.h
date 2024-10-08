#pragma once
#include "StdLib.h"
#include "LexerTypes.h"
#include "AST.h"
#include "Context.h"
#include "Stack.h"

namespace JSLib {
using namespace Std;
class Parser {
private:
    static Parser* m_Instance;
    Parser(Vector<Token*>& tokens) : m_TokensFromLexer(tokens) , m_TokenCounter(*new TokenCounter()) {};
public:
    static Parser* GetInstance(Vector<Token*>& tokens) {
        if (m_Instance == nullptr)
            m_Instance = new Parser(tokens);
        return m_Instance;
    }
    static Parser* GetInstance() {
        if (m_Instance == nullptr) {
            throw std::runtime_error("Parser has not been initialized with tokens.");
        }
        return m_Instance;
    }
    void Analyze();
    ASTNode* GenerateAST(NodeBranchInfo& NodeInfo);
    ASTNode* GeneratePartialAST(NodeBranchInfo& NodeInfo);
    ASTNode* GetTopLevelASTNode() {return m_TopLevelASTNode;};
private:
    IfStatement* CreateNewIfStatement();
    ForStatement* CreateNewForStatement();
    BinaryOpStatement* CreateNewBinaryOpStatement();
    WhileStatement* CreateNewWhileStatement() {};

    void StatementParser(Statement&& statement);
    
    Optional<List<Token*>> ConsumeSpecificSpan(TokenType FirstToken, TokenType LastToken);
    Optional<List<Token*>> ConsumeStatement(StatementType statementType);
    Token* Consume();
    
    Optional<TokenType> PeekFront(int Distance);
    Optional<TokenType> PeekHind(int Distance);
    
    Optional<TokenType> ExpectToken(TokenType tokenType);
    Optional<StatementType> ExpectStatement(StatementType statType);
    
    class Encapsulator {
    public:
        using Variable = int; //variable tanımla
        void InitScope() {}
        void EncapsulateWithScope (NodeBranchInfo& info) {}
        void PushLocal() {}
        class ScopeChecker : public Visitor {
        public:
            bool CheckScopeViolation() {}
        };
        ScopeChecker& checker() {return m_Checker;}
        List<Variable> ExtractLocals() {return Locals.ConvertToList();}
    private:
        Util::Stack<Variable> Locals;
        ScopeChecker m_Checker; 
    };

    void SetupTokenInitializers() {
        auto nultoken = new Token;
        nultoken->Type = TokenType::INVALID;
        NullTokenList.push_back(nultoken);
        };

    ASTContext& ASTcx() {return m_ASTContext;};
private:
    class TokenCounter {
    private:
        List<Token*> m_TokenList;
        Util::Stack<Token*> m_StatementTracerStack;
        List<List<Token*>> m_StatementTokenLists;
        Token* m_CurrentTokenPtr;
        int m_CurrentIndex;
    public:
        List<List<Token*>>& GetStatementTokenLists() {return m_StatementTokenLists;} 
        int GetCurrentIndex() {return m_CurrentIndex;}
        Token* GetCurrentTokenPtr() {return m_CurrentTokenPtr;}
        void PushToStatementTracerStack() {m_StatementTracerStack.Push(m_CurrentTokenPtr);}
        void PopFromStatementTracerStack() {m_StatementTracerStack.Pop();}
        Token* PeekStatementTracerStack() {return m_StatementTracerStack.Peek();}
        void AddExternalToken (TokenType type) {
            auto ExternalToken = new Token;
            ExternalToken->Type = type;
            auto idx = m_TokenList.FindIndex(m_CurrentTokenPtr); 
            m_TokenList.ShiftRightByIndex(idx - 1);
            m_TokenList[idx] = Move(ExternalToken);
        }
        void Initialize(List<Token*>& tokenList) {
            m_TokenList = tokenList;
            m_CurrentTokenPtr = m_TokenList[0];
            m_CurrentIndex = 0;
        }
        void SetCurrentTokenPtr(Token* tokenPtr) {
            m_CurrentTokenPtr = tokenPtr;
        }
        Token* Next() {
            m_CurrentIndex++;
            m_CurrentTokenPtr = m_TokenList.Next(m_TokenList.FindNode(m_CurrentTokenPtr));
            return m_CurrentTokenPtr;
        }
        Token* Peek(int Distance) {
            return m_TokenList[m_CurrentIndex + Distance];
        }
        Token* GotoNextStatement() {
            while (m_CurrentTokenPtr->Type != TokenType::END_OF_STATEMENT)
                m_CurrentTokenPtr = Next();
            return m_CurrentTokenPtr;
        }

    };
    TokenCounter& tokenCounter() {return m_TokenCounter;}
    Encapsulator& encapsulator() {return m_Encapsulator;}
private:
    ASTNode* m_TopLevelASTNode;
    ASTContext m_ASTContext;
    Vector<Token*>& m_TokensFromLexer;
    TokenCounter m_TokenCounter;
    Encapsulator m_Encapsulator;
    ProgramStatement NewProgramStatement;
    List<Token*> NullTokenList;


};

}