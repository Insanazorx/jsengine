
#pragma once
#include "StdLib.h"
#include "LexerTypes.h"
#include "EnumTypes.h"
#include "Visitor.h"
#include "Context.h"
#include "Stack.h"

namespace JSLib {
using namespace Std;
class Statement;
class NodeBranchInfo;
class ASTBuilder;
class ASTNode;
class ParserContext;
class Parser;
    class DummyNode;
    class Node;

class Statement{
protected:
    Statement() {}
public:
    virtual ~Statement() = default;
    static Statement* Create() {return new Statement();}
    virtual StatementType Type() {return m_Type;}
    virtual NodeBranchInfo* ParseTokens(ParserContext* context) {};
    virtual ASTNode* GenerateASTImmediate(ParserContext* context) {}
    Statement(const Statement& other) {
        m_Type = other.m_Type;
        isAnalyzeSet = other.isAnalyzeSet;
        m_Type = other.m_Type;
    }
    
protected:
    std::list<Token*> m_TokenChain;
    bool isAnalyzeSet = false;
    StatementType m_Type;
};

class ProgramStatement : public Statement{
friend class Statement;
private:
    ProgramStatement() {};
    ~ProgramStatement() override = default;
public:
    static ProgramStatement* Create() {
        return new ProgramStatement();
    }
private:
    std::list<Statement*> m_Statements;
};

class ScopeStatement final : public Statement {
friend class Statement;
public:
    ScopeStatement(Token* start, Token* end, StatementType type) : m_Start(start), m_End(end), m_ParentType(type) {}
    ~ScopeStatement() override = default;
    NodeBranchInfo* ParseTokens(ParserContext* context) override {}
private:
    Token* m_Start;
    Token* m_End;
    StatementType m_ParentType;
};

class IfStatement : public Statement {
friend class Statement;
private:    
    IfStatement() = default;
public:
    ~IfStatement() override = default;
    static IfStatement* Create() {return new IfStatement();}
    IfStatement* PushTestTokens(std::list<Token*> tokens) {for (auto token: tokens) m_TestTokens.push_back(token);return this;};
    IfStatement* PushConseqTokens(std::list<Token*> tokens) {for (auto token: tokens) m_ConsequentTokens.push_back(token);return this;};
    IfStatement* ApproveAlternateToken(std::list<Token*> tokens) {for (auto token: tokens) m_AlternateTokens.push_back(token);return this;};
    std::list<Token*>& GetTestTokens() {return m_TestTokens;}
    std::list<Token*>& GetConseqTokens() {return m_ConsequentTokens;}
    std::list<Token*>& GetAlternateTokens() {return m_AlternateTokens;}

    NodeBranchInfo* ParseTokens(ParserContext* context);
    StatementType Type() override {return StatementType::IF_STATEMENT;}
    
private:   
    std::list<Token*> m_TestTokens;
    std::list<Token*> m_ConsequentTokens;
    std::list<Token*> m_AlternateTokens;
};

class ImmediateStatement : public Statement {
friend class Statement;
protected:
    ImmediateStatement() = default;
public:
    ~ImmediateStatement() override = default;
    static ImmediateStatement* Create() {return new ImmediateStatement();}

    virtual ImmediateStatement* AddToChain(Token* TokenToAdd) {m_TokenChain.push_back(TokenToAdd); return this;};
    NodeBranchInfo* ParseTokens(ParserContext* context) override {};
    StatementType Type() override {return StatementType::BINARY_OP_STATEMENT;}
    virtual StatementType subType() {}
    ASTNode* GenerateASTImmediate(ParserContext* context) override {};
};

class BoolStatement final : public ImmediateStatement {
private:
    BoolStatement() = default;
public:
    ~BoolStatement() = default;
    static BoolStatement* TryToCreate(std::list<Token*>) {return new BoolStatement();}//TODO
    StatementType subType() override {return StatementType::BOOL_STATEMENT;}
    ASTNode* GenerateASTImmediate(ParserContext* context) override {}
    ImmediateStatement* AddToChain(Token* TokenToAdd) override {m_TokenChain.push_back(TokenToAdd);}
private:
    std::optional<Token*> NegatorToken;
    Statement* StatementToConvertBoolean;
    bool LastStatementInChain;
};

class VariableStatement : public ImmediateStatement {
private:
    VariableStatement() = default;
public:
    ~VariableStatement() override = default;
    static VariableStatement* Create() {return new VariableStatement;}
    StatementType subType() override {return StatementType::VARIABLE_STATEMENT;}
    ASTNode* GenerateASTImmediate(ParserContext* context) override {}
};

class UnaryOpStatement final : public ImmediateStatement {
private:
    UnaryOpStatement() = default;
public:
    ~UnaryOpStatement() override = default;
    static UnaryOpStatement* Create() {return new UnaryOpStatement;}
    StatementType subType() override {return StatementType::UNARY_OP_STATEMENT;}
    ASTNode* GenerateASTImmediate(ParserContext* context) override {}
};

class AssignmentStatement final : public ImmediateStatement {
private:
    AssignmentStatement() = default;
public:
    ~AssignmentStatement() override = default;
    static AssignmentStatement* TryToCreate(std::list<Token*>) {return new AssignmentStatement();}//TODO
    StatementType subType() override {return StatementType::ASSIGNMENT_STATEMENT;}
    ASTNode* GenerateASTImmediate(ParserContext* context) override {}
};

class VariableDeclarationStatement : public ImmediateStatement {
private:
    VariableDeclarationStatement() = default;
public:
    ~VariableDeclarationStatement() override  = default;
    static VariableDeclarationStatement* Create(){return new VariableDeclarationStatement();}
    StatementType subType() override {return StatementType::VARIABLE_DECLARATION_STATEMENT;}
    ASTNode* GenerateASTImmediate(ParserContext* context) override {}
};


class BinaryOpStatement final : public ImmediateStatement {
private:
    BinaryOpStatement() = default;
public:
    ~BinaryOpStatement() override = default;
    static BinaryOpStatement* Create() {return new BinaryOpStatement();}
    std::list<Token*>& GetTokenChain() {return m_TokenChain;}
    StatementType subType() override {return StatementType::BINARY_OP_STATEMENT;}
    ASTNode* GenerateASTImmediate(ParserContext* context) override;

};

class ForStatement : public Statement {
friend class Statement;
private:
    ForStatement() = default;
public:
    ~ForStatement() override = default;
    
    static ForStatement* Create() {return new ForStatement();}
    ForStatement* PushInitialConditionTokens(std::list<Token*> tokens) {for (auto token: tokens) m_InitialConditionTokens.push_back(token);return this;}
    ForStatement* PushTestTokens(std::list<Token*> tokens) {for (auto token: tokens) m_TestTokens.push_back(token);return this;}
    ForStatement* PushIterationTokens(std::list<Token*> tokens) {for (auto token: tokens) m_IterationTokens.push_back(token);return this;}
    ForStatement* PushConseqTokens(std::list<Token*> tokens) {for (auto token: tokens) m_ConsequentTokens.push_back(token);return this;}
    std::list<Token*>& GetInitialConditionTokens() {return m_InitialConditionTokens;}
    std::list<Token*>& GetTestTokens() {return m_TestTokens;}
    std::list<Token*>& GetIterationTokens() {return m_IterationTokens;}
    std::list<Token*>& GetConseqTokens() {return m_ConsequentTokens;}
    
    
    NodeBranchInfo* ParseTokens(ParserContext* context);
    StatementType Type() override {return StatementType::FOR_STATEMENT;}
private:
    std::list<Token*> m_InitialConditionTokens;
    std::list<Token*> m_TestTokens;
    std::list<Token*> m_IterationTokens;
    std::list<Token*> m_ConsequentTokens;
};

class WhileStatement final : public Statement {
friend class Statement;
private:
    WhileStatement() {};
public:
    ~WhileStatement() override = default;
    static WhileStatement* Create() {return new WhileStatement();}
    NodeBranchInfo* ParseTokens (ParserContext* context) override {};
    StatementType Type() override {return StatementType::WHILE_STATEMENT;}
private:
    std::list<Token*> m_TestTokens;
    std::list<Token*> m_ConsequentTokens;
};

    class Node {
    public:
        explicit Node () : parent(nullptr) {};
        virtual ~Node() {}
    protected:
        Node* parent;
        std::list<Node*> children;
    public:
        Node* Parent() const {return parent;}
        std::list<Node*>& Children() {return children;}
        bool hasParent() const {return !!parent;}
        void AddChild(Node* child) {children.push_back(child);}
        void AddParent(Node* parent) {parent=parent;}
    };
    class DummyNode : public Node {
        friend class DummyTreeBuilder;
        friend class ASTShape;
    public:
        DummyNode() {};
        ~DummyNode() {};

        Statement* statement;

        DummyNode* AddChild(Statement* statement, ASTNode *const immNode)  {
            auto NewDummyNode = new DummyNode;
            NewDummyNode->parent = this;
            NewDummyNode->children.push_back(reinterpret_cast<Node *const>(immNode));

                NewDummyNode->statement = statement;

            return NewDummyNode;
        }
        void AddSpecificChild(DummyNode* child) {
            children.push_back(reinterpret_cast<Node*> (child));
        }
        DummyNode* FindNode() {

        }
    };


class NodeBranchInfo {

public:
    NodeBranchInfo() noexcept = default;
private:
    class ASTShape {
    friend class NodeBranchInfo;
    friend class ASTBuilder;
    //friend class ASTBuilder::ShapeBuilder;
    public:
        ASTShape() = default;
        ~ASTShape() = default;


        class DummyTreeBuilder {
        friend class ASTShape;
        private:
            DummyTreeBuilder() = default;
            ~DummyTreeBuilder()= default;
        public:
            void UpdateInWhichChildBranch() {
                Node* parent = CurrentDummyNode->Parent();
                InWhichChildBranch = parent->Children().FindIndex(CurrentDummyNode);
            } 
            void GotoParent () {
                CurrentDummyNode = dynamic_cast<DummyNode*>(CurrentDummyNode->Parent());
                UpdateInWhichChildBranch();
            }
            void AddChild(Statement* statement, ASTNode* immNode) {
                CurrentDummyNode->AddChild(statement, immNode);
            }
            void AddChild(Statement* statement) {
                CurrentDummyNode->AddChild(statement, nullptr);

            }
            void AddSpecificChild(DummyNode* node) {
                CurrentDummyNode->AddSpecificChild(node);
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
};
public:
    static NodeBranchInfo* Create() {
        return new NodeBranchInfo();
    }
    void SetStatementType(StatementType Type) {
        m_type = Type;
    }
    ASTShape* Shape() {
        return &m_shape;
    }
    DummyNode* ExtractDummyNode() {
        return m_shape.TLNode;
    }
    ~NodeBranchInfo() = default;
private:


    struct LocationInfo {
        vector<int> branchChain;
        int depth;
    };

    LocationInfo m_loc;
    StatementType m_type;
    ASTShape m_shape;

};
    
class ASTNode : public Node {
protected:
    ASTNode() = default;
    ~ASTNode() override = default;
public:
    static ASTNode* Create(){
        return new ASTNode();
    }
    
    virtual int ChildrenCount() {};

    std::string& Raw() { return m_raw; }

    void SetValue(std::string& raw) {
        m_raw = raw;
    }
protected:
    StatementType NodeType;
    std::string m_raw;
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


class BoolStatementNode : public ASTNode {};
class VariableNode : public ASTNode {};
class UnaryOpStatementNode : public ASTNode {};
class AssignmentStatementNode : public ASTNode {};
class VariableDeclarationStatementNode : public ASTNode {};

class BinaryOpStatementNode : public ASTNode{
public:
    static BinaryOpStatementNode* Create() {
        return new BinaryOpStatementNode();
    }
    int ChildrenCount() override {}
    ~BinaryOpStatementNode() override = default;
    void SetSubType(BinaryOpSubType OpType) {m_SubType = OpType;}
    BinaryOpSubType SubType() {return BinaryOpType;}

private:
    BinaryOpStatementNode() = default;
    BinaryOpSubType BinaryOpType;
private:
    BinaryOpSubType m_SubType;
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
friend class Parser;
private:

    class ASTVisitorForBuilding : public Visitor {
    public:
        ASTVisitorForBuilding() {};

        void PlaceNodeAndIterateToChild(ASTNode* NodeToPlace);
        void SetCurrentToTopLevelNode(ASTNode* node) {m_CurrentNode = node; m_TopLevelNode = node;}
        void PushToNodeStack(ASTNode* node) {NodeStackToReturnBack.push_back(node);}
        bool CheckIfThereAreChildren() {};
        bool CheckIfStackContainsElement();
        bool CheckIfMoreThanOneChild();
        bool ReturnToLastStackElement();
        bool SwitchToOtherChild();
        void SwapNode(ASTNode* TreeNode, ASTNode* StrayNode);
        void ReturnToParent() {m_CurrentNode=m_CurrentNode->Parent();}
        ASTNode *CurrentNode() {return dynamic_cast<ASTNode*>(m_CurrentNode);}
    private:
        Node* m_CurrentNode;
        ASTNode* m_TopLevelNode;
        std::list<ASTNode*> NodeStackToReturnBack;
    };
public:
    struct CacheChain { //branchların sistematik olarak iç içe tutulması için
        friend class ImmediateBuilder;
        CacheChain() =default;
        ~CacheChain() = default;
        ASTNode* currentNode;
        CacheChain* UpperChain;
        CacheChain* InnerChain;
        void AllocInnerChain() {
            InnerChain = new CacheChain;
            InnerChain->UpperChain = this;
        }
    };

    class ImmediateBuilder {
    public:
        void SetCurrentNode(ASTNode* node) {m_CurrentNode = node;}
        ASTNode* CurrentNode() {return m_CurrentNode;}
        void ReturnToMainBranch();

        void PushToNodeStack(ASTNode* node) {m_NodeStack.Push(node);}
        ASTNode* PopFromNodeStack() {return m_NodeStack.Pop();}


        void GoTopLevelNodeOfCurrentBranch() {
            while (m_CurrentNode->hasParent()) {
                m_CurrentNode = dynamic_cast<ASTNode *>(m_CurrentNode->Parent());
            }
        }

        ASTNode* GetTopLevelNode(ASTNode* node) {
            while (m_CurrentNode->hasParent()) {
                node = dynamic_cast<ASTNode*>(node->Parent());
            }
        }

        void GoToNumericChild();

        std::list<Token*>& EmptyOneExprFromStack(); //parantezli ifade için

        void LinkInnerParenthesesNodeToOuterOne(CacheChain* CurrentChain);
        void LinkcurrentNodeToMainBranchNode();
        bool hasRemainedOneExprInStack();

        bool ParseInnerParentheses(std::list<Token*>& TokenChain,CacheChain* CurrentChain, ParserContext* context);

        bool isStackEmpty() {return m_NodeStack.IsEmpty();}
        void AddParent(ASTNode* parent) {m_CurrentNode->AddParent(parent);}
        void AddChild(ASTNode* child) {m_CurrentNode->AddChild(child);}

        Util::Stack<ASTNode*>& NodeStack() {return m_NodeStack;}

        Util::Stack<Token*>& TokenStack() {return m_TokenStack;}
    private:
        ASTNode* m_CurrentNode;
        ASTNode* m_LastUsedMainBranchNode;
        Util::Stack<ASTNode*> m_NodeStack;
        Util::Stack<Token*> m_TokenStack;
        CacheChain m_TopLevelCacheChain;

    };
public:
    ASTBuilder() = default;
    ~ASTBuilder() =default;

public:
    ASTVisitorForBuilding& visitor() {return m_Visitor;}
    ImmediateBuilder& immBuilder() {return m_Builder;}
    ASTNode* GenerateScaffoldingByShape();
    void SetBuilderParametersByLookingShape();
    ASTNode* SearchSpecificNode(NodeBranchInfo* info, ASTNode* Current);
    ASTNode* ExtractTopLevelNode();
private:
    NodeBranchInfo& m_NodeBranchInfo;
    ImmediateBuilder m_Builder;
    ASTVisitorForBuilding m_Visitor;
};


}




