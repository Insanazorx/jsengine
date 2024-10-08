#include "LexerTypes.h"

namespace JSLib {
class Statement{
public:
    void operator=(const Statement& other) {
        this->m_Range = other.m_Range;
        this->m_Type = other.m_Type;}
    virtual StatementType Type() {return m_Type;}
protected:
    TokenRange m_Range;
    StatementType m_Type;
};

class ProgramStatement : public Statement{
public:
    ProgramStatement() {};
    ~ProgramStatement() {};
};

class ScopeStatement final : public Statement {
public:
    ScopeStatement(Token* start, Token* end, StatementType type) : m_Start(start), m_End(end), m_ParentType(type) {}
    ~ScopeStatement() {}
private:
    Token* m_Start;
    Token* m_End;
    StatementType m_ParentType;
};

class IfStatement final : public Statement {
public:
    IfStatement() {};
    ~IfStatement() {};
    IfStatement* PushTestTokens(List<Token*> tokens) {for (auto token: tokens) m_TestTokens.push_back(token);return this;};
    IfStatement* PushConseqTokens(List<Token*> tokens) {for (auto token: tokens) m_ConsequentTokens.push_back(token);return this;};
    IfStatement* ApproveAlternateToken(List<Token*> tokens) {for (auto token: tokens) m_AlternateTokens.push_back(token);return this;};
    NodeBranchInfo ParseTokens();
    StatementType Type() override {return StatementType::IF_STATEMENT;}
private:   
    List<Token*> m_TestTokens;
    List<Token*> m_ConsequentTokens;
    List<Token*> m_AlternateTokens;
};

class BinaryOpStatement final : public Statement {
public:
    BinaryOpStatement() {};
    ~BinaryOpStatement() {};
    BinaryOpStatement* AddToChain(Token* TokenToAdd) {m_TokenChain.push_back(TokenToAdd); return this;};  
    NodeBranchInfo ParseTokens();
    StatementType Type() override {return StatementType::BINARY_OP_STATEMENT;}
private:
    List<Token*> m_TokenChain;
};

class ForStatement final : public Statement {
public:
    ForStatement() {};
    ~ForStatement() {};
    ForStatement* PushInitialConditionTokens(List<Token*> tokens) {for (auto token: tokens) m_InitialConditionTokens.push_back(token);return this;}
    ForStatement* PushTestTokens(List<Token*> tokens) {for (auto token: tokens) m_TestTokens.push_back(token);return this;}
    ForStatement* PushIterationTokens(List<Token*> tokens) {for (auto token: tokens) m_IterationTokens.push_back(token);return this;}
    ForStatement* PushConseqTokens(List<Token*> tokens) {for (auto token: tokens) m_ConsequentTokens.push_back(token);return this;}
    NodeBranchInfo ParseTokens();
    StatementType Type() override {return StatementType::FOR_STATEMENT;}
private:
    List<Token*> m_InitialConditionTokens;
    List<Token*> m_TestTokens;
    List<Token*> m_IterationTokens;
    List<Token*> m_ConsequentTokens;
};

class WhileStatement final : public Statement {
public:
    WhileStatement() {};
    ~WhileStatement() {};
    StatementType Type() override {return StatementType::WHILE_STATEMENT;}
private:
    List<Token*> m_TestTokens;
    List<Token*> m_ConsequentTokens;
};
}