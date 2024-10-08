#pragma once
#include "AST.h"
#include "StdLib.h"
namespace JSLib{
using namespace Std;
class Context{};
class FrontendContext : public Context{
public:
    bool AnalyzeOnceActivated() {return m_AnalyzeOnceActivated;}
    void TurnBitAOA(){m_AnalyzeOnceActivated = true;}
private:
    bool m_AnalyzeOnceActivated = false;
};
class ParserContext : public Context {
public:
    Token* CurrentToken() {return m_CurrentTokenPtr;}
    void operator=(ParserContext& other) {
        this->m_CurrentTokenPtr = other.m_CurrentTokenPtr;
    }
private:
    Token* m_CurrentTokenPtr;
    Vector<Token*>& m_Tokens;
};
class ASTContext : public Context {
public:
    ASTContext(ASTBuilder& builder):m_Builder(builder){};
    ASTContext() = default;
    
    

    class InfoMap {
    
    private:
        List<NodeBranchInfo*> m_InfoList;
        List<ASTNode*> m_NodeList;
    };  
    
    bool UpdateContext(NodeBranchInfo const&& Info) {};
    InfoMap& Map() {return m_Map;}
    ASTBuilder& Builder() {return m_Builder;}
    Util::Stack<ASTNode*>& NodeStack() {return m_ASTStack;}
private:
    InfoMap m_Map;
    ASTBuilder m_Builder;
    Util::Stack<ASTNode*> m_ASTStack;
};
}