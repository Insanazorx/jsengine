#ifndef PARSER_H
#define PARSER_H
#include <vector>
#include <optional>
#include <type_traits>
#include <functional>

#include "AST.h"
#include "AST.h"
#include "AST.h"
#include "AST.h"
#include "AST.h"
#include "LexerTypes.h"
#include "Util.h"
#include "Lexer.h"
#include "Forward.h"

#define PRINT_TOKENS(tokens) \
for (auto token : tokens) \
    DEBUG( GREEN_TEXT <<"PARSER : " << FUNCTION_NAME() << RESET_TEXT << " Type of token: " << Lexer::StringifyTokenType(token));

namespace JSLib {

class Parser {
friend class ParserContext;
ALL_STATEMENTS_FRIENDS;

private:

    Parser(const Parser&) = delete;
    Parser& operator=(const Parser&) = delete;
    Parser(Parser&&) = delete;
    Parser(std::vector<Token> tokens, ParserContext* context) :m_MajorTokenCounter(MajorTokenCounter::Create()), m_context(context), m_TokensFromLexer(tokens), m_CurrentTokenCounter(nullptr) {};
    Parser():m_TokensFromLexer(*new std::vector<Token>), m_MajorTokenCounter(nullptr), m_CurrentTokenCounter(nullptr) {};
public:

    static Parser* Initialize(std::vector<Token> tokens, ParserContext* context) {
        if (m_Instance == nullptr) {
            m_Instance = new Parser(tokens, context);
        }
        else {
            m_Instance->m_MajorTokenCounter = MajorTokenCounter::Create();
            m_Instance->m_MajorTokenCounter->SetTokens(tokens);
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
    ASTNode* ParseInGlobalContextFromTokens() {return EnterAnalyzerLoop();}
    ASTNode* ParseFunction() {}
    ASTNode* ValidateSemanticsByVisitor(NodeBranchInfo* NodeInfo);

    ASTNode* GetTopLevelASTNode() {return m_TopLevelASTNode;};
private:
std::variant<ASTNode*, Statement*> AnalyzeImpl(std::optional<std::vector<Token> > tokens, ParserContext *context, AnalyzeMode mode);

    void DecideStatementType(Statement *&statement);

    static void BuildStatement(Statement*& statement, ParserContext* context);

    void BuildIfStatement(Statement*& statement, ParserContext* context);
    void BuildForStatement(Statement*& statement, ParserContext* context);
    void BuildWhileStatement(Statement*& statement, ParserContext* context);
    void BuildFunctionStatement(Statement*& statement, ParserContext* context);
    void BuildReturnStatement(Statement*& statement, ParserContext* context);
    void BuildBinaryOpStatement(Statement*& statement, ParserContext* context);
    void BuildUnaryOpStatement(Statement*& statement, ParserContext* context);
    void BuildVariableDeclarationStatement(Statement*& statement, ParserContext* context);
    void BuildScopeStatement(Statement*& statement, ParserContext* context);

    void DispatchStatement(Statement* statement, ParserContext* context);

    ASTNode* EnterAnalyzerLoop() {return std::get<ASTNode*>(AnalyzeImpl(std::nullopt,m_context, AnalyzeMode::AST_NODE));}

    ASTNode* StatementParser(Statement* statement, ParserContext* context);

    void RulesForIfExpr(Statement *new_statement);
    void RulesForExprWithBracketsAndBraces(Statement *new_statement);
    void RulesForExprWithBracketsAndOptionalBraces(Statement *new_statement);
    void RulesForExprWithBrackets(Statement *new_statement);
    void RulesForExprWithNone(Statement *new_statement);

Token& Consume();
    Token GetPreviousTokenWithoutGoingBack();
    static ExpressionScaffoldingType GetExpressionScaffoldingTypeByLookingKeyword (const TokenType type) {
        switch (type) {
            case TokenType::IF:
                return ExpressionScaffoldingType::IF_EXPR;
            case TokenType::FOR:
                return ExpressionScaffoldingType::EXPR_WITH_BRACKETS_AND_OPTIONAL_BRACES;
            case TokenType::WHILE:
                return ExpressionScaffoldingType::EXPR_WITH_BRACKETS_AND_OPTIONAL_BRACES;
            case TokenType::FUNCTION:
                return ExpressionScaffoldingType::EXPR_WITH_BRACKETS_AND_BRACES;
            case TokenType::SWITCH:
                return ExpressionScaffoldingType::EXPR_WITH_BRACKETS_AND_BRACES;
            case TokenType::CLASS:
                return ExpressionScaffoldingType::EXPR_WITH_BRACKETS_AND_BRACES;
            default:
                return ExpressionScaffoldingType::EXPR_WITH_NONE;
        }
    }

    static void SanitizeEmptyTokens(std::vector<Token>& TokensToSanitize);
    TokenType PeekFront(int Distance);
    TokenType PeekHind(int Distance);
    Token DebugPeek(int Distance); //debug

    bool DecideIfSingleTokenStatement();

    enum class TokenCounterType {MAJOR, MINOR, NOT_SPECIFIED};
    class TokenCounter {
public:
        virtual ~TokenCounter() = default;
    protected:
        TokenCounter() = default;

    protected:
        Stack<TokenType> BracketStack;
        Stack<TokenType> BraceStack;
        std::vector<Token> m_TokenList;
        Stack<Token*> m_StatementTracerStack;
        std::deque<Token> m_NewStatementTokenList;
        Token* m_CurrentTokenPtr {nullptr};
        int m_CurrentIndex {0};
    public:
        //for debug
        std::vector<Token>& GetTokenList() {return m_TokenList;}
        //for debug

        virtual TokenCounterType Type(){return TokenCounterType::NOT_SPECIFIED;}
        void SetTokens(std::vector<Token>& tokens) {m_TokenList = tokens;}
        void PushToNewStatementTokenList(Token& token) {m_NewStatementTokenList.push_back(token);}
        int FindFirstIndexOfToken(TokenType type) {
            for (int i = 0; i < m_TokenList.size(); i++) {
                if (m_TokenList[m_CurrentIndex + i].Type == type) {
                     return m_CurrentIndex + i;
                }
            }
        }
        TokenType PeekSpecific(int Distance) {
            return m_TokenList[m_CurrentIndex + Distance].Type;
        }

        std::deque<Token>& GetStatementTokenList() {return m_NewStatementTokenList;}
        int GetCurrentIndex() {return m_CurrentIndex;}
        Token* GetCurrentTokenPtr() {
            if (m_CurrentTokenPtr == nullptr) {
                m_CurrentTokenPtr = &m_TokenList[0];
                DEBUG(GREEN_TEXT<<"PARSER "<<FUNCTION_NAME()<<RESET_TEXT<<" " << m_CurrentTokenPtr->Lexeme);
            }
            return m_CurrentTokenPtr;
        }
        void PushToStatementTracerStack() {m_StatementTracerStack.Push(m_CurrentTokenPtr);}
        void PopFromStatementTracerStack() {m_StatementTracerStack.Pop();}
        Token* PeekStatementTracerStack() {return m_StatementTracerStack.Peek();}

        void SetCurrentTokenPtr(Token* tokenPtr) {
            m_CurrentTokenPtr = tokenPtr;
        }
        Token& Next() {
            SetCurrentTokenPtr(&m_TokenList[++m_CurrentIndex]);
            return m_TokenList[m_CurrentIndex];
        }
        Token GetPrevTokenWithoutGoingBack() {
            return m_TokenList[m_CurrentIndex-1];
        }
        Token* Peek(const int Distance) {

            return &m_TokenList[m_CurrentIndex + Distance];
        }
        Token* GotoNextStatement() {
            while (m_CurrentTokenPtr->Type != TokenType::SEMICOLON)
                m_CurrentTokenPtr = &Next();
            return m_CurrentTokenPtr;
        }

        void PushToBracketStack(const TokenType type) {
            BracketStack.Push(type);
        }

        void PushToBraceStack(const TokenType type) {
            BraceStack.Push(type);
        }

        //pass the type of current token, use with loops, only use with R_BRACE or R_BRACKET, while false increment the index
        bool CheckWhetherClosingTokenComeYet(const TokenType tokenType) {
            if (BracketStack.IsEmpty() && BraceStack.IsEmpty()) {
                return false;
            }
            if (tokenType == TokenType::R_BRACE)
                if (BraceStack.Peek() == TokenType::L_BRACE) {

                    BraceStack.Pop();

                    if (BraceStack.IsEmpty()) {
                        return true;
                    }
            }
            if (tokenType == TokenType::R_BRACKET) {
                if (BracketStack.Peek() == TokenType::L_BRACKET) {
                    BracketStack.Pop();

                    if (BracketStack.IsEmpty()) {
                        return true;
                    }
                }
            }
            return false;
        }

    };

    class MajorTokenCounter : public TokenCounter {
    private:
        MajorTokenCounter() = default;
    public:
        ~MajorTokenCounter() override = default;
        static TokenCounter* Create() {
            return new MajorTokenCounter();
        }
    private:
        TokenCounterType Type() override {return TokenCounterType::MAJOR;}
    };

    class MinorTokenCounter : public TokenCounter {
        private:
        MinorTokenCounter() = default;
        public:
        ~MinorTokenCounter() override = default;
        static TokenCounter* Create() {
            return new MinorTokenCounter();
        }
        private:
        TokenCounterType Type() override {return TokenCounterType::MINOR;}
    };

    TokenCounter* tokenCounter() {return m_CurrentTokenCounter;}
public:
    ParserContext* Context() {return m_context;}
    void SetContext(ParserContext* context) {m_context = context;}
    ImmediateBuilder* immBuilder() {return &m_ImmediateBuilder;}
private:
    ASTNode* m_TopLevelASTNode;
    std::vector<Token>& m_TokensFromLexer;
    ImmediateBuilder m_ImmediateBuilder;
    TokenCounter* m_MajorTokenCounter;
    TokenCounter* m_MinorTokenCounter;
    TokenCounter* m_CurrentTokenCounter;
    ProgramStatement* NewProgramStatement;
    std::vector<NodeBranchInfo*> m_NodeBranchInfoVector;
    ParserContext* m_context;
    static Parser* m_Instance;

};

}


#endif