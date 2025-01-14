#ifndef PARSER_H
#define PARSER_H
#include <vector>
#include <optional>
#include <type_traits>
#include <functional>

#include "LexerTypes.h"
#include "AST.h"
#include "Context.h"
#include "Util.h"
#include "Lexer.h"

#define PRINT_TOKENS(tokens) \
for (auto token : tokens) \
    DEBUG( GREEN_TEXT <<"PARSER : " << FUNCTION_NAME() << RESET_TEXT << " Type of token: " << Lexer::StringifyTokenType(token));

namespace JSLib {
    class Statement;
    class ParserContext;
    class IfStatement;
    class ForStatement;
    class ImmediateStatement;
    class ASTBuilder;
    class ProgramStatement;
    class NodeBranchInfo;

    template <typename T>
    concept CallableReturnsVoid = std::invocable<T> && std::same_as<std::invoke_result_t<T>, void>;
    template<typename T>
    concept CallableReturnsInt = std::invocable<T,TokenType> && std::same_as<std::invoke_result_t<T,TokenType>, int>;

class Parser {
friend class ParserContext;
    friend class Statement;
    friend class IfStatement;
    friend class ForStatement;
    friend class ImmediateStatement;
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
    ASTNode* GenerateAST(NodeBranchInfo* NodeInfo);
    ASTNode* GeneratePartialAST(NodeBranchInfo* NodeInfo);

    ASTNode* GetTopLevelASTNode() {return m_TopLevelASTNode;};
private:

    ASTNode* RecognizeStatementOrRedirectNode(std::optional<std::vector<Token>>& tokens, ParserContext* context);

    ASTNode* EnterAnalyzerLoop() {return RecognizeStatementOrRedirectNode(*new std::optional<vector<Token>>{nullopt} ,m_context);}

    ASTNode* StatementParser(Statement* statement, ParserContext* context);

    IfStatement* CreateNewIfStatement();
    WhileStatement* CreateNewWhileStatement();

    BinaryOpStatement* CreateNewBinaryOpStatement();
    //WhileStatement* CreateNewWhileStatement() {};


    template <CallableReturnsInt T, CallableReturnsVoid F>
    std::optional<std::vector<Token>> ConsumeSpecificSpan(F &&StartingCondition, T &&PermanentCondition);

    Token Consume();
    Token GetPreviousTokenWithoutGoingBack();

    static void SanitizeEmptyTokens(std::vector<Token>& TokensToSanitize);
    TokenType PeekFront(int Distance);
    TokenType PeekHind(int Distance);
    Token DebugPeek(int Distance); //debug

    bool ExpectToken (TokenType TokenTypeToExpect);
    bool ExpectTokenAtAnywhere(TokenType TokenTypeToExpect);

    bool DecideIfSingleTokenStatement();



public:
    ASTBuilder* Builder() {
        if (m_Builder == nullptr)
            m_Builder = new ASTBuilder();
        return m_Builder;
    };
private:

    enum class TokenCounterType {MAJOR, MINOR, NOT_SPECIFIED};
    class TokenCounter {
public:
        virtual ~TokenCounter() = default;
    protected:
        TokenCounter() = default;

    protected:
        Stack<TokenType> BracketOrBraceStack;
        std::vector<Token> m_TokenList;
        Stack<Token*> m_StatementTracerStack;
        std::vector<std::vector<Token>> m_StatementTokenLists;
        Token* m_CurrentTokenPtr {nullptr};
        int m_CurrentIndex {0};
    public:
        //for debug
        std::vector<Token>& GetTokenList() {return m_TokenList;}
        //for debug

        virtual TokenCounterType Type(){return TokenCounterType::NOT_SPECIFIED;}
        void SetTokens(std::vector<Token>& tokens) {m_TokenList = tokens;}

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

        std::vector<std::vector<Token>>& GetStatementTokenLists() {return m_StatementTokenLists;}
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
            while (m_CurrentTokenPtr->Type != TokenType::END_OF_STATEMENT)
                m_CurrentTokenPtr = &Next();
            return m_CurrentTokenPtr;
        }

        void PushToBracketOrBraceStack(const TokenType type) {
            BracketOrBraceStack.Push(type);
        }

        bool CheckIfRightBracketOrBraceProperAndPairThem(const TokenType tokenType) {
            if (BracketOrBraceStack.IsEmpty()) {
                return false;
            }
            DEBUG(LOGGER_BANNER(TOKEN_COUNTER)<<"Stack Size: " << BracketOrBraceStack.Size());
            if (BracketOrBraceStack.Peek() == TokenType::L_BRACE && tokenType == TokenType::R_BRACE) {

                BracketOrBraceStack.Pop();

                if (BracketOrBraceStack.IsEmpty()) {
                    return true;
                }
            }
            if (BracketOrBraceStack.Peek() == TokenType::L_PARENTHESES && tokenType == TokenType::R_PARENTHESES) {

                BracketOrBraceStack.Pop();

                if (BracketOrBraceStack.IsEmpty()) {
                    return true;
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
    ParserContext* Context() {return m_context;}
    void SetContext(ParserContext* context) {m_context = context;}
private:
    ASTNode* m_TopLevelASTNode;
    ASTBuilder* m_Builder;
    std::vector<Token>& m_TokensFromLexer;
    TokenCounter* m_MajorTokenCounter;

    TokenCounter* m_MinorTokenCounter;
    TokenCounter* m_CurrentTokenCounter;
    ProgramStatement* NewProgramStatement;
    std::vector<NodeBranchInfo*> m_NodeBranchInfovector;
    ParserContext* m_context;
    static Parser* m_Instance;

};

}


#endif