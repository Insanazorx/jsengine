

#pragma once

#include "LexerTypes.h"
#include "EnumTypes.h"
#include "Visitor.h"
#include "Context.h"
#include "Util.h"

namespace JSLib {

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
    virtual ASTNode* GenerateASTImmediate(ParserContext* context) {return nullptr;}
    Statement(const Statement& other) {
        m_Type = other.m_Type;
        isAnalyzeSet = other.isAnalyzeSet;
        m_Type = other.m_Type;
    }
    virtual bool isImmediate(){return false;}
    std::vector<Token>& GetTokenChain() {return m_TokenChain;}
protected:
    std::vector<Token> m_TokenChain;
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
    std::vector<Statement*> m_Statements;
};

class ScopeStatement final : public Statement {
friend class Statement;
public:
    ScopeStatement(Token start, Token end, StatementType type) : m_Start(start), m_End(end), m_ParentType(type) {}
    ~ScopeStatement() override = default;
    NodeBranchInfo* ParseTokens(ParserContext* context) override {}
private:
    Token m_Start;
    Token m_End;
    StatementType m_ParentType;
};

class IfStatement : public Statement {
friend class Statement;
private:
    IfStatement() = default;
public:
    ~IfStatement() override = default;
    static IfStatement* Create() {return new IfStatement();}
    IfStatement* PushTestTokens(std::vector<Token>&& tokens) {for (auto token: tokens) m_TestTokens.push_back(token);return this;};
    IfStatement* PushConseqTokens(std::vector<Token>&& tokens) {for (auto token: tokens) m_ConsequentTokens.push_back(token);return this;};
    IfStatement* ApproveAlternateToken(std::vector<Token>&& tokens) {for (auto token: tokens) m_AlternateTokens.push_back(token);return this;};
    std::vector<Token>& GetTestTokens() {return m_TestTokens;}
    std::vector<Token>& GetConseqTokens() {return m_ConsequentTokens;}
    std::vector<Token>& GetAlternateTokens() {return m_AlternateTokens;}

    NodeBranchInfo* ParseTokens(ParserContext* context);
    StatementType Type() override {return StatementType::IF_STATEMENT;}

private:
    std::vector<Token> m_TestTokens;
    std::vector<Token> m_ConsequentTokens;
    std::vector<Token> m_AlternateTokens;
};

class ImmediateStatement : public Statement {
friend class Statement;
protected:
    ImmediateStatement() = default;
public:
    ~ImmediateStatement() override = default;
    static ImmediateStatement* Create() {return new ImmediateStatement();}

    virtual ImmediateStatement* AddToChain(Token TokenToAdd) {m_TokenChain.push_back(TokenToAdd); return this;};
    NodeBranchInfo* ParseTokens(ParserContext* context) override {};
    bool isImmediate() override {return true;}
    ASTNode* GenerateASTImmediate(ParserContext* context) {};

};


class UnaryOpStatement final : public ImmediateStatement {
private:
    UnaryOpStatement() = default;
public:
    ~UnaryOpStatement() override = default;
    static UnaryOpStatement* Create() {return new UnaryOpStatement;}
    StatementType Type() override {return StatementType::UNARY_OP_STATEMENT;}
    ASTNode* GenerateASTImmediate(ParserContext* context) {}
};

class AssignmentStatement final : public ImmediateStatement {
private:
    AssignmentStatement() = default;
public:
    ~AssignmentStatement() override = default;
    static AssignmentStatement* TryToCreate(std::vector<Token>) {return new AssignmentStatement();}//TODO
    StatementType Type() override {return StatementType::ASSIGNMENT_STATEMENT;}
    ASTNode* GenerateASTImmediate(ParserContext* context) {}
};

class VariableDeclarationStatement : public ImmediateStatement {
private:
    VariableDeclarationStatement() = default;
public:
    ~VariableDeclarationStatement() override  = default;
    static VariableDeclarationStatement* Create(){return new VariableDeclarationStatement();}
    StatementType Type() override {return StatementType::VARIABLE_DECLARATION_STATEMENT;}
    ASTNode* GenerateASTImmediate(ParserContext* context) {}
};


class BinaryOpStatement final : public ImmediateStatement {
private:
    BinaryOpStatement() = default;
public:
    ~BinaryOpStatement() override = default;
    static BinaryOpStatement* Create() {return new BinaryOpStatement();}
    std::vector<Token>& GetTokenChain() {return m_TokenChain;}
    StatementType Type() override {return StatementType::BINARY_OP_STATEMENT;}
    ASTNode* GenerateASTImmediate(ParserContext* context);

};

class ForStatement : public Statement {
friend class Statement;
private:
    ForStatement() = default;
public:
    ~ForStatement() override = default;

    static ForStatement* Create() {return new ForStatement();}
    ForStatement* PushInitialConditionTokens(std::vector<Token> tokens) {for (auto token: tokens) m_InitialConditionTokens.push_back(token);return this;}
    ForStatement* PushTestTokens(std::vector<Token> tokens) {for (auto token: tokens) m_TestTokens.push_back(token);return this;}
    ForStatement* PushIterationTokens(std::vector<Token> tokens) {for (auto token: tokens) m_IterationTokens.push_back(token);return this;}
    ForStatement* PushConseqTokens(std::vector<Token> tokens) {for (auto token: tokens) m_ConsequentTokens.push_back(token);return this;}
    std::vector<Token>& GetInitialConditionTokens() {return m_InitialConditionTokens;}
    std::vector<Token>& GetTestTokens() {return m_TestTokens;}
    std::vector<Token>& GetIterationTokens() {return m_IterationTokens;}
    std::vector<Token>& GetConseqTokens() {return m_ConsequentTokens;}


    NodeBranchInfo* ParseTokens(ParserContext* context);
    StatementType Type() override {return StatementType::FOR_STATEMENT;}
private:
    std::vector<Token> m_InitialConditionTokens;
    std::vector<Token> m_TestTokens;
    std::vector<Token> m_IterationTokens;
    std::vector<Token> m_ConsequentTokens;
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
    std::vector<Token> m_TestTokens;
    std::vector<Token> m_ConsequentTokens;
};

    class ASTNode {
    protected:
        ASTNode() = default;
        ASTNode(std::string raw) : m_raw(std::move(raw)) {}
    public:
        virtual ~ASTNode() = default;
        static ASTNode* Create(){
            return new ASTNode();
        }

        static ASTNode* Create(const std::string& raw) {
            return new ASTNode(raw);
        }



        std::string& Raw() { return m_raw; }

        void SetValue(std::string&& raw) {
            m_raw = raw;
        }
        void SetValue(const std::string& raw) {
            m_raw = raw;
        }

        bool hasParent() {return !!m_Parent;}
        std::vector<ASTNode*>& Children() {return m_Children;}
        ASTNode* Parent() {return m_Parent;}
        ASTNode* GetChild(int index) {return m_Children[index];}
        ASTNode* AddChild(ASTNode* child) {m_Children.push_back(child);return this;}
        void AddParent(ASTNode* parent) {m_Parent = parent;}

        virtual bool isIdentifierNode() {return false;}
        virtual bool isNumericNode() {return false;}
        virtual bool isScopeNode() {return false;}
        virtual bool isBinaryOpNode() {return false;}
        virtual bool isUnaryOpNode() {return false;}
        virtual bool isAssignmentNode() {return false;}
        virtual bool isVariableDeclarationNode() {return false;}
        virtual bool isIfNode() {return false;}
        virtual bool isWhileNode() {return false;}
        virtual bool isForNode() {return false;}
        virtual bool isForInNode() {return false;}
        virtual bool isForOfNode() {return false;}
        virtual bool isForAwaitOfNode() {return false;}
        virtual bool isFunctionNode() {return false;}
        virtual bool isArrowFunctionNode() {return false;}
        virtual bool isAsyncFunctionNode() {return false;}
        virtual bool isGeneratorFunctionNode() {return false;}
        virtual bool isReturnNode() {return false;}


    protected:

        ASTNode* m_Parent {nullptr};
        std::string m_raw;
        bool BuilderCheck;

    private:
        std::vector<ASTNode*> m_Children;
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
                ASTNode* parent = CurrentDummyNode->Parent();
                InWhichChildBranch = FindIndex<ASTNode*>(parent->Children(),CurrentDummyNode);
            }
            void GotoParent () {
                CurrentDummyNode = CurrentDummyNode->Parent();
                UpdateInWhichChildBranch();
            }

            void AddChild(ASTNode* node) {
                CurrentDummyNode->AddChild(node);
            }
        private:
            ASTNode* CurrentDummyNode;
            int InWhichChildBranch;
        };
    public:
        DummyTreeBuilder& Builder() {
            return m_Builder;
        }
    private:
        ASTNode* TLNode;
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
    ASTNode* ExtractDummyNode() {
        return m_shape.TLNode;
    }
private:


    StatementType m_type;
    ASTShape m_shape;

};



class ScopeNode : public ASTNode {
public:
    static ScopeNode* Create(){
        return new ScopeNode();
    }
    ~ScopeNode() = default;

    bool isScopeNode() override {return true;}

private:
    std::vector<ASTNode*> m_Children;
    ScopeNode() = default;

};

class IdentifierNode : public ASTNode {
    public:
    static IdentifierNode* Create() {
        return new IdentifierNode();
    }
    ~IdentifierNode() = default;
    bool isIdentifierNode() override {return true;}
    private:
    IdentifierNode() = default;
};
class UnaryOpNode : public ASTNode {};
class AssignmentNode : public ASTNode {};
class VariableDeclarationNode : public ASTNode {};

class BinaryOpNode : public ASTNode{
public:
    static BinaryOpNode* Create() {
        return new BinaryOpNode();
    }

    ~BinaryOpNode() override = default;

    bool isBinaryOpNode() override {return true;}

    void SetSubType(BinaryOpSubType OpType) {m_SubType = OpType;}

    nlohmann::json toJson() override {
        nlohmann::json jsonObj;
        jsonObj["raw"] = Raw();
        jsonObj["subType"] = StringifyBinaryOpSubType(m_SubType);
        jsonObj["lhs"] = LhsNode->toJson();
        jsonObj["rhs"] = RhsNode->toJson();
        return jsonObj;
    }

    BinaryOpSubType SubType() {return m_SubType;}

    void SetLhs(ASTNode* lhs) {LhsNode = lhs;}
    void SetRhs(ASTNode* rhs) {RhsNode = rhs;}

private:
    BinaryOpNode() = default;

    BinaryOpSubType m_SubType;
    ASTNode* LhsNode;
    ASTNode* RhsNode;

};
    class BreakNode : public ASTNode {};
    class ContinueNode : public ASTNode {};

class IfNode : public ASTNode {
public:
    static IfNode* Create() {
        return new IfNode();
    }

private:
    IfNode() = default;
    ~IfNode() = default;
private:
    ASTNode* Condition;
    ASTNode* Consequent;
    ASTNode* Alternate;

};
class ForNode : public ASTNode {
public:
    static ForNode* Create() {
        return new ForNode();
    }

private:
    ForNode() = default;
    ~ForNode() = default;
};
class WhileNode : public ASTNode {
public:
    static WhileNode* Create() {
        return new WhileNode();
    }

private:
    WhileNode() = default;
    ~WhileNode() = default;
};

    class ReturnNode : public ASTNode {};

    class FunctionNode : public ASTNode {
        public:
        static FunctionNode* Create() {
            return new FunctionNode();
        }
        ~FunctionNode() = default;

        private:
        FunctionNode() = default;
        ScopeNode* m_FunctionScopeNode;

    };
    class ASTVisitor : public Visitor {

    public:
        ~ASTVisitor() override = default;

        //virtual visitors of ASTNode Sub Types
        virtual void visit(ASTNode &visit) = 0;
        virtual void visit(IdentifierNode &visit) = 0;
        virtual void visit(UnaryOpNode &visit) = 0;
        virtual void visit(AssignmentNode &visit) = 0;
        virtual void visit(VariableDeclarationNode &visit) = 0;
        virtual void visit(BinaryOpNode &visit) = 0;
        virtual void visit(ScopeNode &visit) = 0;
        virtual void visit(IfNode &visit) = 0;
        virtual void visit(ForNode &visit) = 0;
        virtual void visit(WhileNode &visit) = 0;
        virtual void visit(FunctionNode &visit) = 0;
        virtual void visit(ReturnNode &visit) = 0;
    };

class ASTBuilder{
friend class Parser;
private:

    class ASTSpecializerVisitor : public ASTVisitor {
    public:
        ASTSpecializerVisitor() = default;

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

        void visit(ASTNode &visit) override;
        void visit(IdentifierNode &visit) override;
        void visit(UnaryOpNode &visit) override;
        void visit(AssignmentNode &visit) override;
        void visit(VariableDeclarationNode &visit) override;
        void visit(BinaryOpNode &visit) override;
        void visit(ScopeNode &visit) override;
        void visit(IfNode &visit) override;
        void visit(ForNode &visit) override;
        void visit(WhileNode &visit) override;
        void visit(FunctionNode &visit) override;
        void visit(ReturnNode &visit) override;

    private:
        ASTNode* m_CurrentNode;
        ASTNode* m_TopLevelNode;
        std::vector<ASTNode*> NodeStackToReturnBack;
    };
public:
    struct CacheChain { //branchların sistematik olarak iç içe tutulması için
        CacheChain() =default;
        ~CacheChain() = default;
        ASTNode* CurrentTLNodeOfBranch;
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
                m_CurrentNode = m_CurrentNode->Parent();
            }
        }

        ASTNode* GetTopLevelNode(ASTNode* node) {
            while (m_CurrentNode->hasParent()) {
                node = dynamic_cast<ASTNode*>(node->Parent());
            }
        }

        void GoToNumericChild();

        std::vector<Token>& EmptyOneExprFromStack(); //parantezli ifade için

        void LinkInnerParenthesesNodeToOuterOne(CacheChain* CurrentChain);
        void LinkCurrentTLNodeOfBranchToMainBranchNode();
        bool hasRemainedOneExprInStack();

        bool ParseInnerParentheses(std::vector<Token>& TokenChain,CacheChain* CurrentChain, ParserContext* context);

        bool isStackEmpty() {return m_NodeStack.IsEmpty();}
        void AddParent(ASTNode* parent) {m_CurrentNode->AddParent(parent);}
        void AddChild(ASTNode* child) {m_CurrentNode->AddChild(child);}
        CacheChain* tlChain() {
            return &m_TopLevelCacheChain;
        }
        Stack<ASTNode*>& NodeStack() {return m_NodeStack;}

        Stack<Token>& TokenStack() {return m_TokenStack;}
    private:
        ASTNode* m_CurrentNode;
        ASTNode* m_LastUsedMainBranchNode;
        Stack<ASTNode*> m_NodeStack;
        Stack<Token> m_TokenStack;
        CacheChain m_TopLevelCacheChain;

    };
public:
    ASTBuilder() : m_Builder(new ImmediateBuilder()), m_Visitor(new ASTSpecializerVisitor()) {};
    ~ASTBuilder() =default;

public:
    ASTSpecializerVisitor* visitor() {return m_Visitor;}
    ImmediateBuilder* immBuilder() {return m_Builder;}
    ASTNode* GenerateScaffoldingByShape();
    void SetBuilderParametersByLookingShape();
    ASTNode* SearchSpecificNode(NodeBranchInfo* info, ASTNode* Current);
    ASTNode* ExtractTopLevelNode();
private:
    NodeBranchInfo* m_NodeBranchInfo;
    ImmediateBuilder* m_Builder;
    ASTSpecializerVisitor* m_Visitor;
};


}




