

#pragma once

#include <nlohmann/json.hpp>
#include <curl/curl.h>
#include "LexerTypes.h"
#include "EnumTypes.h"
#include "Visitor.h"
#include "Context.h"
#include "Util.h"


namespace JSLib {
    class ASTVisitor;

    class Statement;
class NodeBranchInfo;
class ASTBuilder;
class ASTNode;
class ParserContext;
class Parser;;
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
    NodeBranchInfo* ParseTokens (ParserContext* context) override {}
    StatementType Type() override {return StatementType::WHILE_STATEMENT;}
private:
    std::vector<Token> m_TestTokens;
    std::vector<Token> m_ConsequentTokens;
};

    class FunctionStatement : public Statement {
    private:
        FunctionStatement() = default;
    public:
        ~FunctionStatement() override = default;
        static FunctionStatement* Create() {
            return new FunctionStatement;
        }
        NodeBranchInfo* ParseTokens (ParserContext* context) override {}
        StatementType Type() override {return StatementType::FUNCTION_STATEMENT;}
    }
    ;

    class ASTNode {
    protected:
        ASTNode() = default;
        explicit ASTNode(std::string raw) : m_raw(std::move(raw)) {}
    public:
        virtual ~ASTNode() =default;

        static ASTNode* Create(){
            return new ASTNode();
        }

        static ASTNode* Create(const std::string& raw) {
            return new ASTNode(raw);
        }

        virtual nlohmann::json toJson() {
            nlohmann::json jsonObj;
            jsonObj["raw"] = m_raw;
            jsonObj["children"] = nlohmann::json::array();
            for (auto child : m_Children) {
                jsonObj["children"].push_back(child->toJson());
            }

            return jsonObj;
        }

        virtual ASTNode* Lhs() {}
        virtual ASTNode* Rhs() {}
        virtual void RemoveLhs() {}
        virtual void RemoveRhs() {}
        virtual void SetLhs(ASTNode* lhs) {};//for BinaryOpNodes
        virtual void SetRhs(ASTNode* rhs) {};//for BinaryOpNodes
        virtual void SetNumericValueFromRaw() {}

        std::string& Raw() { return m_raw;}

        void SetValue(std::string&& raw) {
            m_raw = raw;
        }
        void SetValue(const std::string& raw) {
            m_raw = raw;
        }
        void RemoveParent() {m_Parent = nullptr;}
        bool hasParent() {return !!m_Parent;}
        std::vector<ASTNode*>& Children() {return m_Children;}
        ASTNode* Parent() {return m_Parent;}
        ASTNode* GetChild(int index) {return m_Children[index];}
        ASTNode* AddChild(ASTNode* child) {m_Children.push_back(child);return this;}
        void AddParent(ASTNode* parent) {m_Parent = parent;}

        virtual bool isVariableNode() {return false;}
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
        bool BuilderCheck {false};

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

class NumericNode : public ASTNode {
private:
    NumericNode() = default;
public:
    static NumericNode* Create() {
        return new NumericNode();
    }
    ~NumericNode() override = default;
    void SetNumericValueFromRaw() override {
        m_NumericValue = std::stoi(Raw());
    }
    void SetNumericValue(int num) {m_NumericValue = num;}
    int NumericValue() const  {return m_NumericValue;}
    bool isNumericNode() override {return true;}

    nlohmann::json toJson() override {
        nlohmann::json jsonObj;
        jsonObj["raw"] = Raw();
        jsonObj["numericValue"] = m_NumericValue;
        return jsonObj;
    }

private:
    int m_NumericValue {0};
};

class VariableNode : public ASTNode {
    public:
    static VariableNode* Create() {
        return new VariableNode();
    }

    nlohmann::json toJson() override {
        nlohmann::json jsonObj;
        jsonObj["raw"] = Raw();
        return jsonObj;
    }
    ~VariableNode() = default;
    bool isVariableNode() override {return true;}
    private:
    VariableNode() = default;
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

        if (LhsNode)
            jsonObj["lhs"] = LhsNode->toJson();
        else throw std::runtime_error("LhsNode is nullptr at node with value: " + Raw() + " and Parent: " + Parent()->Raw());

        if (RhsNode)
            jsonObj["rhs"] = RhsNode->toJson();
        else throw std::runtime_error("RhsNode is nullptr at node with value: " + Raw() + " and Parent: " + Parent()->Raw());

        return jsonObj;
    }

    BinaryOpSubType SubType() {return m_SubType;}
    void RemoveLhs() override {LhsNode = nullptr;}
    void RemoveRhs() override {RhsNode = nullptr;}
    void SetLhs(ASTNode* lhs) override {LhsNode = lhs;}
    void SetRhs(ASTNode* rhs) override {RhsNode = rhs;}
    ASTNode* Lhs() override {return LhsNode;}
    ASTNode* Rhs() override {return RhsNode;}

private:
    BinaryOpNode() = default;

    BinaryOpSubType m_SubType;
    ASTNode* LhsNode {nullptr};
    ASTNode* RhsNode {nullptr};

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
        std::vector<VariableNode*> m_arguments;
        std::vector<ASTNode*> m_children;
    };
    class ASTVisitor : public Visitor {

    public:
        ~ASTVisitor() override = default;

        //virtual visitors of ASTNode Sub Types
        virtual void visit(ASTNode &visit) = 0;
        virtual void visit(VariableNode &visit) = 0;
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

    class SpecializerVisitor : public ASTVisitor {
    public:
        SpecializerVisitor() = default;

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

        void visit(ASTNode &visit) override {};
        void visit(VariableNode &visit) override {};
        void visit(UnaryOpNode &visit) override {};
        void visit(AssignmentNode &visit) override{};
        void visit(VariableDeclarationNode &visit) override{};
        void visit(BinaryOpNode &visit) override{};
        void visit(ScopeNode &visit) override{};
        void visit(IfNode &visit) override{};
        void visit(ForNode &visit) override{};
        void visit(WhileNode &visit) override{};
        void visit(FunctionNode &visit) override{};
        void visit(ReturnNode &visit) override{};

    private:
        ASTNode* m_CurrentNode;
        ASTNode* m_TopLevelNode;
        std::vector<ASTNode*> NodeStackToReturnBack;
    };
public:

    class ImmediateBuilder {
    public:

        //For binary ops

        std::shared_ptr<std::vector<Token>> ConvertInfixToPostfix(const std::vector<Token> &TokensToConvert,
                                                                  ParserContext *context,
                                                                  std::unordered_map<std::string, BinaryOpSubType>
                                                                  & OpTypeMap);
        ASTNode* GenerateASTFromPostfix(std::shared_ptr<std::vector<Token>> PostfixToASTTokens,
                                        ParserContext* context,
                                        std::unordered_map<std::string, BinaryOpSubType>& OpTypeMap);

        void SetCurrentNode(ASTNode* node) {m_CurrentNode = node;}
        ASTNode* CurrentNode() {return m_CurrentNode;}

        ASTNode* PeekCurrentTLNodeOfBranch() {
            ASTNode* current = m_CurrentNode;
            while (current->hasParent()) {
                current = current->Parent();
            }
            return current;
        }

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

        void AddRhs(ASTNode* child, ASTNode* parent) {
            child->AddParent(parent);
            parent->SetRhs(child);
        }
        void AddLhs(ASTNode* child, ASTNode* parent) {
            child->AddParent(parent);
            parent->SetLhs(child);
        }


        Stack<Token>& OperatorStack() {return m_OperatorStack;}
    private:
        ASTNode* m_CurrentNode;
        ASTNode* m_LastUsedMainBranchNode;
        Stack<Token> m_OperatorStack;
    };
public:
    ASTBuilder() : m_Builder(new ImmediateBuilder()), m_Visitor(new SpecializerVisitor()) {};
    ~ASTBuilder() =default;

public:
    SpecializerVisitor* visitor() {return m_Visitor;}
    ImmediateBuilder* immBuilder() {return m_Builder;}
    ASTNode* GenerateScaffoldingByShape();
    void SetBuilderParametersByLookingShape();
    ASTNode* SearchSpecificNode(NodeBranchInfo* info, ASTNode* Current);
    ASTNode* ExtractTopLevelNode();
private:
    NodeBranchInfo* m_NodeBranchInfo;
    ImmediateBuilder* m_Builder;
    SpecializerVisitor* m_Visitor;
};


}




