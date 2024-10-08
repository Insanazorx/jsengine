#pragma once
#include "StdLib.h"
#include "LexerTypes.h"
#include "StatementTypes.h"
#include "Visitor.h"

namespace JSLib {
using namespace Std;
class Statement;
class NodeBranchInfo;
class ASTBuilder;
class ASTNode;


class Scope{
public:
    Scope() = default;
    ~Scope() {};
private:
    Statement* OwnerOfScope;
    uint64_t idx;
    uint64_t LastLowerIdx = 0;
    uint64_t IndentationLevel;
    Scope* UpperScope;
    List<Scope*> LowerScope;
public:
    uint64_t AllocLowerScope() {
        auto NewScope = new Scope;
        NewScope->idx = LastLowerIdx++;
        LowerScope.push_back(NewScope);
        return NewScope->idx;
    };
    Scope* operator[](int index){ 
        for (auto LowerScopeMap : LowerScope) {
            if (LowerScopeMap->idx == index) return LowerScopeMap;
            else continue;
        }
    }
    void push_back(Scope* Scope) {
        LowerScope.push_back(new class Scope);
    }
    void SetOwnerOfScope(Statement* oos) {OwnerOfScope = oos;}
};
struct TokenRange {
        List<Token*> Tokens;
        int Length;
    };
class Statement{
public:
    void operator=(const Statement& other) {
        this->m_Range = other.m_Range;
        this->m_Type = other.m_Type;}
    virtual StatementType Type() {return m_Type;}
    virtual NodeBranchInfo ParseTokens() = 0;
    virtual ~Statement() = default;
    
    
protected:
    TokenRange m_Range;
    StatementType m_Type;
};

class ProgramStatement : public Statement{
friend class Statement;
private:
    ProgramStatement() {};
    ~ProgramStatement() {};
public:
    static ProgramStatement* Create() {
        return new ProgramStatement();
    }
    NodeBranchInfo ParseTokens() override {
        for (auto statement : m_Statements) {
            statement->ParseTokens();
        }
    }
private:
    List<Statement*> m_Statements;
};

class ScopeStatement final : public Statement {
friend class Statement;
public:
    ScopeStatement(Token* start, Token* end, StatementType type) : m_Start(start), m_End(end), m_ParentType(type) {}
    ~ScopeStatement() {}
    NodeBranchInfo& ParseTokens() override {TODO();};
private:
    Token* m_Start;
    Token* m_End;
    StatementType m_ParentType;
};

class IfStatement final : public Statement {
friend class Statement;
public:
    IfStatement() = default;
    ~IfStatement();
    IfStatement* PushTestTokens(List<Token*> tokens) {for (auto token: tokens) m_TestTokens.push_back(token);return this;};
    IfStatement* PushConseqTokens(List<Token*> tokens) {for (auto token: tokens) m_ConsequentTokens.push_back(token);return this;};
    IfStatement* ApproveAlternateToken(List<Token*> tokens) {for (auto token: tokens) m_AlternateTokens.push_back(token);return this;};
    NodeBranchInfo& ParseTokens();
    StatementType Type() override {return StatementType::IF_STATEMENT;}
private:   
    List<Token*> m_TestTokens;
    List<Token*> m_ConsequentTokens;
    List<Token*> m_AlternateTokens;
};

class BinaryOpStatement final : public Statement {
friend class Statement;
public:
    BinaryOpStatement() = default;
    ~BinaryOpStatement();
    BinaryOpStatement* AddToChain(Token* TokenToAdd) {m_TokenChain.push_back(TokenToAdd); return this;};  
    NodeBranchInfo& ParseTokens();
    StatementType Type() override {return StatementType::BINARY_OP_STATEMENT;}
private:
    List<Token*> m_TokenChain;
};

class ForStatement final : public Statement {
friend class Statement;
public:
    ForStatement() = default;
    ~ForStatement();
    ForStatement* PushInitialConditionTokens(List<Token*> tokens) {for (auto token: tokens) m_InitialConditionTokens.push_back(token);return this;}
    ForStatement* PushTestTokens(List<Token*> tokens) {for (auto token: tokens) m_TestTokens.push_back(token);return this;}
    ForStatement* PushIterationTokens(List<Token*> tokens) {for (auto token: tokens) m_IterationTokens.push_back(token);return this;}
    ForStatement* PushConseqTokens(List<Token*> tokens) {for (auto token: tokens) m_ConsequentTokens.push_back(token);return this;}
    NodeBranchInfo& ParseTokens();
    StatementType Type() override {return StatementType::FOR_STATEMENT;}
private:
    List<Token*> m_InitialConditionTokens;
    List<Token*> m_TestTokens;
    List<Token*> m_IterationTokens;
    List<Token*> m_ConsequentTokens;
};

class WhileStatement final : public Statement {
friend class Statement;
private:
    WhileStatement() {};
    ~WhileStatement() {};
public:
    NodeBranchInfo& ParseTokens () override {TODO();return {};};
    StatementType Type() override {return StatementType::WHILE_STATEMENT;}
private:
    List<Token*> m_TestTokens;
    List<Token*> m_ConsequentTokens;
};



class NodeBranchInfo {
public:
    NodeBranchInfo operator*() {
        return *this;
    }
private:
    class ASTShape {
    friend class NodeBranchInfo;
    friend class ASTBuilder;
    //friend class ASTBuilder::ShapeBuilder;
    private:
        ASTShape() = default;
        ~ASTShape() = default;
    public:
        static ASTShape* Create() {
            return new ASTShape();
        }
        class DummyNode {
        friend class DummyTreeBuilder;
        friend class ASTShape;
        private:
            DummyNode() = default;
            ~DummyNode() = default; 
            DummyNode* Parent;
            List<DummyNode*> Children;
            Statement* statement;
            DummyNode* AddChild(Statement* statement) {
                auto NewDummyNode = new DummyNode;
                NewDummyNode->Parent = this;
                NewDummyNode->statement = statement;
                return NewDummyNode;
            }
        };
        class DummyTreeBuilder {
        friend class ASTShape;
        private:
            DummyTreeBuilder() = default;
            ~DummyTreeBuilder()= default;
        public:
            void UpdateInWhichChildBranch() {
                auto& parent = CurrentDummyNode->Parent;
                InWhichChildBranch = parent->Children.FindIndex(CurrentDummyNode);
            } 
            void GotoParent () {
                CurrentDummyNode = CurrentDummyNode->Parent;
                UpdateInWhichChildBranch();
            }
            void AddChild(Statement* statement) {
                CurrentDummyNode->AddChild(statement);
            }
            void SetCurrentStatement(Statement* statement){
                CurrentDummyNode->statement = statement; 
            }
        private:
            DummyNode* CurrentDummyNode;
            int InWhichChildBranch;
        };
    public:
        DummyTreeBuilder& Builder() {
            return m_Builder;
        }
    private:
        DummyNode* TLNode;
        DummyTreeBuilder m_Builder;
        void Init() {
            TLNode = new DummyNode();
        }
};
public:
    static NodeBranchInfo* Create() {
        return new NodeBranchInfo;
    }
    void SetStatementType(StatementType type) {
        type = Type;
    }
    
private:
    NodeBranchInfo() = default;
    ~NodeBranchInfo() = default;
    StatementType Type;
    ASTShape Shape;
    ASTNode* TopLevelNode; 
};
    
class ASTNode {
protected:
    ASTNode() = default;
    virtual ~ASTNode() noexcept = default;
public:
    
    ASTNode* Parent() {return m_parent;}
    bool hasParent() {return !!m_parent;}
    
    virtual int ChildrenCount() = 0;

private:
    StatementType NodeType;
    Optional<String> m_value;
    String m_raw;
    ASTNode* m_parent;
    bool BuilderCheck;
};

class ScopeNode : public ASTNode {
public:
    static ScopeNode* Create(){
        return new ScopeNode();
    }
    int ChildrenCount() override {}
private:
    ScopeNode() = default;
    ~ScopeNode() = default;
};

class BinaryOpStatementNode : public ASTNode{
public:
    static BinaryOpStatementNode* Create() {
        return new BinaryOpStatementNode();
    }
    int ChildrenCount() override {}
private:
    BinaryOpStatementNode() = default;
    ~BinaryOpStatementNode() = default;
private:
    ASTNode* LhsNode;
    ASTNode* RhsOrStatementNode;
    
};
class IfStatementNode : public ASTNode {
public:
    static IfStatementNode* Create() {
        return new IfStatementNode();
    }
    int ChildrenCount() override {}
private:
    IfStatementNode() = default;
    ~IfStatementNode() = default;
private:

};
class ForStatementNode : public ASTNode {
public:
    static ForStatementNode* Create() {
        return new ForStatementNode();
    }
    int ChildrenCount() override {}
private:
    ForStatementNode() = default;
    ~ForStatementNode() = default;
};
class ScopeNode : public ASTNode {
public:
    static ScopeNode* Create() {
        return new ScopeNode();
    }
    int ChildrenCount() override {}
private:
    ScopeNode() = default;
    ~ScopeNode() = default;
};
class WhileStatementNode : public ASTNode {
public:
    static WhileStatementNode* Create() {
        return new WhileStatementNode();
    }
    int ChildrenCount() override {}
private:
    WhileStatementNode() = default;
    ~WhileStatementNode() = default;
};


class ASTBuilder{

private:
    class ShapeBuilder{
    public:
        ShapeBuilder() = default;

    friend class ASTBuilder;
    private:
        NodeBranchInfo& m_Info;
    public:
    //  void SetBuilder(ASTBuilder* builder) {m_Builder = builder;}
        void SetInfo(NodeBranchInfo& info) {m_Info = info;}
        void AddChild();
        void SetMappings();
    };
    class ASTVisitorForBuilding : public Visitor {
    public:
        ASTVisitorForBuilding() {};

        void PlaceNodeAndIterateToChild(ASTNode* NodeToPlace);
        void SetCurrentToTopLevelNode(ASTNode* node) {m_CurrentNode = node; m_TopLevelNode = node;}
        void PushToNodeStack(ASTNode* node) {NodeStackToReturnBack.push_back(node);}
        bool CheckIfThereAreChildren();
        bool CheckIfStackContainsElement();
        bool CheckIfMoreThanOneChild();
        bool ReturnToLastStackElement();
        bool SwitchToOtherChild();
        void SwapNode(ASTNode* TreeNode, ASTNode* StrayNode);
        void ReturnToParent() {m_CurrentNode=m_CurrentNode->Parent();}
        ASTNode* CurrentNode() {return m_CurrentNode;}
    private:
        ASTNode* m_CurrentNode;
        ASTNode* m_TopLevelNode;
        List<ASTNode*> NodeStackToReturnBack;
    };
private:   
    ASTBuilder(NodeBranchInfo info) : m_NodeBranchInfo(info) {};
    ASTBuilder();
    ~ASTBuilder() {};

public:
    static ASTBuilder* Instance() {
        if (!m_Instance)
            m_Instance = new ASTBuilder();
        return m_Instance;
    }
    ShapeBuilder& shapeBuilder() {return m_ShapeBuilder;}
    ASTVisitorForBuilding& visitor() {return m_Visitor;}
    ASTNode* GenerateScaffoldingByShape();
    void SetBuilderParametersByLookingShape();
    ASTNode* SearchSpecificNode(NodeBranchInfo& info, ASTNode* Current);
    ASTNode* ExtractTopLevelNode();
private:
    static ASTBuilder* m_Instance;
    NodeBranchInfo& m_NodeBranchInfo;
    ShapeBuilder m_ShapeBuilder;
    ASTVisitorForBuilding m_Visitor;
};

}
