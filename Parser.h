#ifndef PARSER_H
#define PARSER_H

#include "StdLib.h"
#include "LexerTypes.h"
#include "AST.h"
#include "Context.h"
#include "Stack.h"

namespace JSLib {


using namespace Std;

    class Statement;
    class ParserContext;
    class IfStatement;
    class ForStatement;
    class ImmediateStatement;
    class ASTBuilder;
    class ProgramStatement;
    class NodeBranchInfo;

class Parser {
friend class ParserContext;
private:

    Parser(const Parser&) = delete;
    Parser& operator=(const Parser&) = delete;
    Parser(Parser&&) = delete;
    Parser(vector<Token*>& tokens, ParserContext* context) :m_MajorTokenCounter(new TokenCounter()), m_context(context), m_TokensFromLexer(tokens), m_CurrentTokenCounter(nullptr) {};
    Parser():m_TokensFromLexer(*new vector<Token*>), m_MajorTokenCounter(nullptr), m_CurrentTokenCounter(nullptr) {};
public:

    static Parser* Initialize(vector<Token*>& tokens, ParserContext* context) {
        if (m_Instance == nullptr) {
            m_Instance = new Parser(tokens, context);
        }
        else {
            m_Instance->m_MajorTokenCounter = new TokenCounter();
            m_Instance->m_CurrentTokenCounter = m_Instance->m_MajorTokenCounter;
        }
        m_Instance->m_TokensFromLexer = tokens;
        m_Instance->m_context = context;
        return m_Instance;
    }
    static Parser* Instance() {
        if (m_Instance == nullptr) {
            m_Instance = new Parser();
        }
        return m_Instance;
    }
    ASTNode* Analyze() {return EnterAnalyzerLoop();}

    ASTNode* GenerateAST(NodeBranchInfo* NodeInfo);
    ASTNode* GeneratePartialAST(NodeBranchInfo* NodeInfo);

    ASTNode* GetTopLevelASTNode() {return m_TopLevelASTNode;};

    // bunun aslında private olması lazım ama function ptr çekmem için gerekli
    Node* RecognizeStatementOrRedirectNode(std::optional<std::list<Token*>>& tokens, ParserContext* context);
private:
    ASTNode* EnterAnalyzerLoop() {return dynamic_cast<ASTNode*>(RecognizeStatementOrRedirectNode(*new std::optional<std::list<Token *>> ,m_context));}

    Node* StatementParser(Statement* statement, ParserContext* context);

    IfStatement* CreateNewIfStatement();
    ForStatement* CreateNewForStatement();
    ImmediateStatement* CreateNewBinaryOpStatement();
    //WhileStatement* CreateNewWhileStatement() {};

    std::optional<std::list<Token*>> ConsumeSpecificSpan(TokenType FirstToken, TokenType LastToken);
    std::optional<std::list<Token*>> ConsumeStatement(StatementType statementType);
    Token* Consume();
    Token* GetPreviousTokenWithoutGoingBack();
    
    std::optional<TokenType> PeekFront(int Distance);
    std::optional<TokenType> PeekHind(int Distance);
    
    std::optional<TokenType> ExpectToken(TokenType tokenType);
    std::optional<StatementType> ExpectStatement(StatementType statType);

    void SetupTokenInitializers() {
        auto nultoken = new Token();
        nultoken->Type = TokenType::INVALID;
        NullTokenstd::list.push_back(nultoken);
        };
public:
    ASTBuilder* Builder() {return m_Builder;};
private:
    class TokenCounter {
    private:
        std::list<Token*> m_Tokenstd::list;
        Util::Stack<Token*> m_StatementTracerStack;
        std::list<std::list<Token*>> m_StatementTokenstd::lists;
        Token* m_CurrentTokenPtr;
        int m_CurrentIndex;
    public:
        void SetTokens(std::list<Token*> tokens) {m_Tokenstd::list = tokens;}
        std::list<std::list<Token*>>& GetStatementTokenstd::lists() {return m_StatementTokenstd::lists;}
        int GetCurrentIndex() {return m_CurrentIndex;}
        Token* GetCurrentTokenPtr() {return m_CurrentTokenPtr;}
        void PushToStatementTracerStack() {m_StatementTracerStack.Push(m_CurrentTokenPtr);}
        void PopFromStatementTracerStack() {m_StatementTracerStack.Pop();}
        Token* PeekStatementTracerStack() {return m_StatementTracerStack.Peek();}
        void AddExternalToken (TokenType type) {
            auto ExternalToken = new Token;
            ExternalToken->Type = type;
            auto idx = m_Tokenstd::list.FindIndex(m_CurrentTokenPtr);
            m_Tokenstd::list.ShiftRightByIndex(idx - 1);
            m_Tokenstd::list[idx] = std::move(ExternalToken);
        }
        void Initialize(std::list<Token*>& tokenstd::list) {
            m_Tokenstd::list = tokenstd::list;
            m_CurrentTokenPtr = m_Tokenstd::list[0];
            m_CurrentIndex = 0;
        }
        void SetCurrentTokenPtr(Token* tokenPtr) {
            m_CurrentTokenPtr = tokenPtr;
        }
        Token* Next() {
            m_CurrentIndex++;
            m_CurrentTokenPtr = m_Tokenstd::list.Next(m_Tokenstd::list.FindNode(m_CurrentTokenPtr));
            return m_CurrentTokenPtr;
        }
        Token* GetPrevTokenWithoutGoingBack() {
            return m_Tokenstd::list.Prev(m_Tokenstd::list.FindNode(m_CurrentTokenPtr));
        }
        Token* Peek(int Distance) {
            return m_Tokenstd::list[m_CurrentIndex + Distance];
        }
        Token* GotoNextStatement() {
            while (m_CurrentTokenPtr->Type != TokenType::END_OF_STATEMENT)
                m_CurrentTokenPtr = Next();
            return m_CurrentTokenPtr;
        }

    };
    TokenCounter* tokenCounter() {return m_CurrentTokenCounter;}
    ParserContext* Context() {return m_context;}
    void SetContext(ParserContext* context) {m_context = context;}
private:
    ASTNode* m_TopLevelASTNode;
    ASTBuilder* m_Builder;
    vector<Token*>& m_TokensFromLexer;
    TokenCounter* m_MajorTokenCounter;
    TokenCounter* m_MinorTokenCounter;
    TokenCounter* m_CurrentTokenCounter;
    ProgramStatement* NewProgramStatement;
    std::list<NodeBranchInfo*> m_NodeBranchInfolist;
    ParserContext* m_context;
    static Parser* m_Instance;

};

}


#endif