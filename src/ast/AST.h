

#pragma once

#include <random>
#include <sstream>
#include <nlohmann/json.hpp>
#include "../frontend/LexerTypes.h"
#include "../frontend/EnumTypes.h"
#include "../Util.h"
#include "../Validator.h"
#include "../frontend/Forward.h"
#include "../interpreter/Bytecode/BytecodeGenerator.h"


namespace js {
    class Validator;
    class ASTVisitor;

    class StatementConverter;
class NodeBranchInfo;
class ASTBuilder;
class ASTNode;
class ParserContext;
class Parser;
    class Node;

    template <typename T>
    concept DerivedFromStatement = std::is_base_of<Statement, T>::value;


class Statement{

ALL_STATEMENTS_FRIENDS;

protected:
    Statement() {
        m_id = GenerateUUID();
    }
public:
    virtual ~Statement() = default;
    static Statement* Create() {return new Statement();}

    virtual void PushToken(Token& token) {m_tokens.push_back(token);}
    virtual TokenPosition Validate(Validator&) {return {-1,-1}; }

    virtual StatementType Type() {return StatementType::INVALID_STATEMENT;}
    virtual NodeBranchInfo* ParseTokens(ParserContext* context) {};
    virtual ASTNode* GenerateASTImmediate(ParserContext* context) {return nullptr;}

    virtual ErrorOr<std::vector<Token>,SyntaxError> MakeTokenVector() {return ErrorOr<std::vector<Token>,SyntaxError>::Ok(m_tokens);}

    virtual Statement* FocusOnTestStatement(ParserContext *context) {};
    virtual Statement* FocusOnConsequentStatement(ParserContext *context) {};
    virtual Statement* FocusOnAlternateStatement(ParserContext *context) {};



    virtual void SetInitExpr(ImmediateStatement *expression) {}
    virtual void SetUpdateExpr(ImmediateStatement* expression) {}
    virtual void SetTestExpr(ImmediateStatement *expression) {}
    virtual void SetConsequentExpr(Statement* expression) {}
    virtual void SetConsequentImm(ImmediateStatement* expression) {}
    virtual void SetAlternateExpr(Statement* expression) {}

    virtual ImmediateStatement* TestExpr() {return nullptr;}
    virtual ImmediateStatement* ConsequentImm() {return nullptr;}
    virtual Statement* ConsequentExpr() {return nullptr;}
    virtual Statement* AlternateExpr() {return nullptr;}
    virtual Statement* InitExpr() {return nullptr;}
    virtual Statement* UpdateExpr() {return nullptr;}
    virtual void forEach(const std::function<void(Statement *)>) {}

    template <DerivedFromStatement S>
    [[nodiscard]] S* ConvertTo() {
        auto tokens = std::move(m_tokens);
        auto new_statement = S::Create();
        new_statement->m_tokens = std::move(tokens);
        return new_statement;
    }


    Statement(const Statement& other) {
        m_tokens = other.m_tokens;
    }

    virtual bool isImmediate(){return false;}

    virtual void awaitExpression(ParserContext* context) {

    }

    std::vector<Token>& Tokens() {return m_tokens;}
    virtual int AwaitingExpressionCount() {return 0;} //-1 means no limit

    [[nodiscard]] std::string ID() const {
        return m_id;
    }

    void SetParentID(const std::string& id) {
        m_id_of_parent = id;
    }

    [[nodiscard]] std::optional<std::string> ParentID() const {
        return m_id_of_parent;
    }

    bool SearchForSpecificHolder ( StatementHolderType type, const std::string& parent_id) {
        if (m_id_of_parent.has_value() && m_id_of_parent.value() != parent_id) {
            return false;
        }
        for (auto holder: m_holders) {
            if (holder->holder_type == type) {
                return true;
            }
        }
        return false;
    }

protected:
    std::string m_id;
    std::optional<std::string> m_id_of_parent;
    std::vector<Token> m_tokens;
    std::vector<StatementHolder*> m_holders;

    static std::string GenerateUUID() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<uint64_t> dis(0, std::numeric_limits<uint64_t>::max());

        std::stringstream ss;
        ss << std::hex << std::setw(16) << std::setfill('0') << dis(gen);
        return ss.str();
    }



};

    class BraceStatement: public Statement {

    protected:
        BraceStatement() = default;
    public:
        static BraceStatement* Create() {return new BraceStatement();}
        ~BraceStatement() override = default;
        NodeBranchInfo* ParseTokens(ParserContext* context) override {}
        StatementType CheckWhetherScopeOrObjectByLookingTokens();
        int AwaitingExpressionCount() override {return -1;}
    };


class ScopeStatement final : public BraceStatement {

private:
    ScopeStatement() = default;
public:
    static ScopeStatement* Create() {return new ScopeStatement();}
    ~ScopeStatement() override = default;
    NodeBranchInfo* ParseTokens(ParserContext* context) override;
    void forEach(const std::function<void(Statement*)> Callable) override {
        for (auto statement: m_Statements) {
            Callable(statement);
        }
    };

    int AwaitingExpressionCount() override {return -1;}
    StatementType Type() override {return StatementType::SCOPE_STATEMENT;}
private:
    std::vector<Statement*> m_Statements;
};

    class ObjectStatement : public BraceStatement {
    private:
        ObjectStatement() = default;
    public:
        static ObjectStatement* Create() {return new ObjectStatement();}
        ~ObjectStatement() override = default;
        NodeBranchInfo* ParseTokens(ParserContext* context) override;
        StatementType Type() override {return StatementType::OBJECT_STATEMENT;}
    };

    class IfStatement : public Statement {
friend class Statement;
private:
    IfStatement() = default;
public:
    ~IfStatement() override = default;
    static IfStatement* Create() {return new IfStatement();}
        Statement* FocusOnTestStatement(ParserContext *context) override;

        void SetTest(ImmediateStatement* test) {m_test = test;}
    void SetConsequent(Statement* consequent) {m_consequent = consequent;}
    void SetAlternate(Statement* alternate) {m_alternate = alternate;}

    ImmediateStatement* Test() {return m_test;}
    Statement* Consequent() {return m_consequent;}
    Statement* Alternate() {return m_alternate;}

    TokenPosition Validate(Validator& validator) override {return validator.Visit(this);}

    NodeBranchInfo* ParseTokens(ParserContext* context) override;
    StatementType Type() override {return StatementType::IF_STATEMENT;}
        int AwaitingExpressionCount() override {return 3;}
private:
    ImmediateStatement* m_test {nullptr};
    Statement* m_consequent {nullptr};
    Statement* m_alternate {nullptr};
};

    class BracketStatement : public Statement {
    private:
        BracketStatement() = default;
    public:
        ~BracketStatement() override = default;
        static BracketStatement* Create() {return new BracketStatement();}
        NodeBranchInfo* ParseTokens (ParserContext* context) override {}
        StatementType Type() override {return StatementType::BRACKET_STATEMENT;}
        int AwaitingExpressionCount() override {return -1;}

    };


class ImmediateStatement : public Statement {
friend class Statement;

protected:
    ImmediateStatement() = default;
public:
    ~ImmediateStatement() override = default;
    static ImmediateStatement* Create() {return new ImmediateStatement();}

    virtual void SetLhs (ImmediateStatement* lhs) {}
    virtual void SetRhs (ImmediateStatement* rhs) {}

    NodeBranchInfo* ParseTokens(ParserContext* context) override {};
    bool isImmediate() override {return true;}
    ASTNode* GenerateASTImmediate(ParserContext* context) {};
    TokenPosition Validate(Validator& validator) override {return validator.Visit(this);}
    ErrorOr<std::vector<Token>, SyntaxError> MakeTokenVector() override {}
    int AwaitingExpressionCount() override {return 2;}
};

class UnaryOpStatement final : public ImmediateStatement {

private:
    UnaryOpStatement() = default;
public:
    ~UnaryOpStatement() override = default;
    static UnaryOpStatement* Create() {return new UnaryOpStatement;}
    StatementType Type() override {return StatementType::UNARY_OP_STATEMENT;}
    ASTNode* GenerateASTImmediate(ParserContext* context) {}
    TokenPosition Validate(Validator& validator) override {return validator.Visit(this);}
    int AwaitingExpressionCount() override {return 2;}

private:
    ImmediateStatement* m_expression {nullptr};
    SingleTokenStatement* m_op {nullptr};
};

class AssignmentStatement final : public ImmediateStatement {

    private:
    AssignmentStatement() = default;
public:
    ~AssignmentStatement() override = default;
    static AssignmentStatement* Create() {return new AssignmentStatement();}
    StatementType Type() override {return StatementType::ASSIGNMENT_STATEMENT;}
    ASTNode* GenerateASTImmediate(ParserContext* context) override;
    TokenPosition Validate(Validator& validator) override {return validator.Visit(this);}
    int AwaitingExpressionCount() override {return 2;}

private:
    ImmediateStatement* m_lvalue {nullptr};
    ImmediateStatement* m_rvalue {nullptr};
};

class VariableDeclarationStatement : public ImmediateStatement {

private:
    VariableDeclarationStatement() = default;
public:
    ~VariableDeclarationStatement() override  = default;
    static VariableDeclarationStatement* Create(){return new VariableDeclarationStatement();}
    StatementType Type() override {return StatementType::VARIABLE_DECLARATION_STATEMENT;}
    ASTNode* GenerateASTImmediate(ParserContext* context) {}
    TokenPosition Validate(Validator& validator) override {return validator.Visit(this);}
    int AwaitingExpressionCount() override {return 1;}

    
private:
    Statement* m_variable {nullptr};
};


class BinaryOpStatement final : public ImmediateStatement {

private:
    BinaryOpStatement() = default;
public:
    ~BinaryOpStatement() override = default;
    static BinaryOpStatement* Create() {return new BinaryOpStatement();}

    StatementType Type() override {return StatementType::BINARY_OP_STATEMENT;}
    ASTNode* GenerateASTImmediate(ParserContext* context) override;

    ErrorOr<std::vector<Token>, SyntaxError> MakeTokenVector() override {};

    TokenPosition Validate(Validator& validator) override {return validator.Visit(this);}
    int AwaitingExpressionCount() override {return 3;}

private:
    ImmediateStatement* m_lhs {nullptr};
    ImmediateStatement* m_op {nullptr};
    Statement* m_rhs {nullptr};
};

    class SingleTokenStatement : public ImmediateStatement {

    private:
        SingleTokenStatement(Token token) : m_Token(token){};
    public:
        ~SingleTokenStatement() override = default;
        static SingleTokenStatement* Create(Token token) {return new SingleTokenStatement(token);}
        StatementType Type() override {return StatementType::VARIABLE_STATEMENT;}

    private:
        Token m_Token;

    };


class ForStatement : public Statement {
private:
    ForStatement() = default;

public:
    ~ForStatement() override = default;

    static ForStatement* Create() {return new ForStatement();}

    void SetInit(Statement* init) {m_init = init;}
    void SetTest(Statement* test) {m_test = test;}
    void SetUpdate(Statement* update) {m_update = update;}
    void SetBody(Statement* body) {m_body = body;}

    TokenPosition Validate(Validator& validator) override {return validator.Visit(this);}
    NodeBranchInfo* ParseTokens(ParserContext* context) {};
    StatementType Type() override {return StatementType::FOR_STATEMENT;}
    int AwaitingExpressionCount() override {return 4;}

private:
    Statement* m_init {nullptr};
    Statement* m_test {nullptr};
    Statement* m_update {nullptr};
    Statement* m_body {nullptr};
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
    int AwaitingExpressionCount() override {return 2;}
    TokenPosition Validate(Validator& validator) override {return validator.Visit(this);}

private:
    Statement* m_test {nullptr};
    Statement* m_body {nullptr};
};

    class FunctionStatement : public Statement {

        private:

        FunctionStatement() = default;
    public:
        ~FunctionStatement() override = default;
        static FunctionStatement* Create() {
            return new FunctionStatement;
        }

        void AddArgument(SingleTokenStatement* argument) {m_arguments.push_back(argument);}
        void AddBodyStatement(ScopeStatement* statement) {m_body_statements = statement;}
        void SetReturnStatement(ReturnStatement* statement) {m_return_statement = statement;}

        std::vector<SingleTokenStatement*> Arguments() {return m_arguments;}
        ScopeStatement* BodyStatements() {return m_body_statements;}
        ReturnStatement* ReturnStatement() {return m_return_statement;}

        TokenPosition Validate(Validator& validator) override {return validator.Visit(this);}
        NodeBranchInfo* ParseTokens (ParserContext* context) override {}
        StatementType Type() override {return StatementType::FUNCTION_STATEMENT;}
        int AwaitingExpressionCount() override {return -1;}


    private:
        std::vector<SingleTokenStatement*> m_arguments {nullptr};
        ScopeStatement* m_body_statements {nullptr};
        class ReturnStatement* m_return_statement {nullptr};

    };

    class ReturnStatement : public Statement {

    private:
        ReturnStatement() = default;
    public:
        ~ReturnStatement() override = default;
        static ReturnStatement* Create() {
            return new ReturnStatement();
        }
        NodeBranchInfo* ParseTokens (ParserContext* context) override;
        StatementType Type() override {return StatementType::RETURN_STATEMENT;}

        TokenPosition Validate(Validator& validator) override {return validator.Visit(this);}

        void SetConsequentImm (ImmediateStatement* expression) override {m_expression = expression;}
        ImmediateStatement* ConsequentImm() override {return m_expression;}

    private:
        ImmediateStatement* m_expression {nullptr};

    };

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
            jsonObj["NodeType"] = "ASTNode";
            return jsonObj;
        }

        virtual TokenPosition Validate(Validator& validator) {return {-1,-1};}

        virtual void SetTestNode(ASTNode* testNode) {}
        virtual void SetConsequentNode(ASTNode* consequentNode) {}
        virtual void SetAlternateNode(ASTNode* alternateNode) {}
        virtual ASTNode* Lhs() {}
        virtual ASTNode* Rhs() {}
        virtual void RemoveLhs() {}
        virtual void RemoveRhs() {}
        virtual void SetLhs(ASTNode* lhs) {};//for BinaryOpNodes
        virtual void SetRhs(ASTNode* rhs) {};//for BinaryOpNodes
        virtual void SetNumericValueFromRaw() {}

        std::string& Raw() { return m_raw;}

        virtual void SetValue(std::string&& raw) {
            m_raw = raw;
        }

        virtual void SetValue(const std::string& raw) {
            m_raw = raw;
        }
        void RemoveParent() {m_Parent = nullptr;}
        bool hasParent() {return !!m_Parent;}
        ASTNode* Parent() {return m_Parent;}
        void AddParent(ASTNode* parent) {m_Parent = parent;}


        virtual Interpreter::GenerateBytecodeResult GenerateBytecode(Interpreter::BytecodeGenerator& generator) {
            return Interpreter::GenerateBytecodeResult::UNSPECIFIED_NODE;
        }

        virtual bool isVariableNode() {return false;}
        virtual bool isNumericNode() {return false;}
        virtual bool isBlockNode() {return false;}
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
        virtual bool isFunctionDeclaration() {return false;}
        virtual bool isCallExpression() {return false;}
        virtual bool isArrowFunctionNode() {return false;}
        virtual bool isAsyncFunctionNode() {return false;}
        virtual bool isGeneratorFunctionNode() {return false;}
        virtual bool isReturnNode() {return false;}


    protected:

        ASTNode* m_Parent {nullptr};
        std::string m_raw;
        bool BuilderCheck {false};

    };


class ProgramNode : public ASTNode {
private:
    ProgramNode() = default;
public :
    static ProgramNode* Create() {
        return new ProgramNode();
    }
    ~ProgramNode() override = default;

    void AddChild(ASTNode* child) {
        m_children.push_back(child);
        child->AddParent(this);
    }

    nlohmann::json toJson() override {
        nlohmann::json jsonObj;
        jsonObj["NodeType"] = "Program";
        for (const auto& child : m_children) {
            jsonObj["children"].push_back(child->toJson());
        }
        return jsonObj;
    }
    Interpreter::GenerateBytecodeResult GenerateBytecode(Interpreter::BytecodeGenerator& generator) override{
                return Interpreter::GenerateBytecodeResult::UNSPECIFIED_NODE;
            }
private:
    std::vector<ASTNode*> m_children;
};

class BlockNode : public ASTNode {
public:
    static BlockNode* Create(){
        return new BlockNode();
    }
    ~BlockNode() override = default;

    bool isBlockNode() override {return true;}

    void AddChild(ASTNode* child) {
        m_children.push_back(child);
    }

    nlohmann::json toJson() override {
        nlohmann::json jsonObj;
        jsonObj["NodeType"] = "BlockStatement";
        for (const auto& child : m_children) {
            jsonObj["children"].push_back(child->toJson());
        }
        return jsonObj;
    }


private:
    std::vector<ASTNode*> m_children;
    BlockNode() = default;

};
    class ValueNode: public ASTNode {
    private:
        ValueNode() = default;
    public:
        static ValueNode* Create() {
            return new ValueNode();
        }
        ~ValueNode() override = default;
        void SetValue(std::string&& raw) override {m_raw = raw;}
        void SetValue(const std::string& raw) override{m_raw = raw;}
        bool isValueNode() {return true;}
        nlohmann::json toJson() override {
            nlohmann::json jsonObj;
            jsonObj["raw"] = m_raw;
            jsonObj["NodeType"] = "Value";
            return jsonObj;
        }
    private:
        std::string m_raw;
    };


class StringNode : public ASTNode {

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
        jsonObj["NodeType"] = "Variable";
        return jsonObj;
    }
    ~VariableNode() = default;
    bool isVariableNode() override {return true;}

private:
    VariableNode() = default;
};
class UnaryOpNode : public ASTNode {};
class AssignmentNode : public ASTNode {
private:
    AssignmentNode() = default;
public:
    static AssignmentNode* Create() {
        return new AssignmentNode();
    }
    ~AssignmentNode() override = default;
    bool isAssignmentNode() override {return true;}
    nlohmann::json toJson() override {
        nlohmann::json jsonObj;
        jsonObj["raw"] = Raw();
        jsonObj["lvalue"] = m_lvalue->toJson();
        jsonObj["rvalue"] = m_rvalue->toJson();
        return jsonObj;
    }

    void SetLvalue(ASTNode* lvalue) {m_lvalue = lvalue; lvalue->AddParent(this);}
    void SetRvalue(ASTNode* rvalue) {m_rvalue = rvalue; rvalue->AddParent(this);}

private:
    ASTNode* m_lvalue {nullptr};
    ASTNode* m_rvalue {nullptr};
};

    enum class DeclarationKind {
        CONST, VAR, LET, INVALID
    };
class VariableDeclarationNode : public ASTNode {
public:
    static VariableDeclarationNode* Create() {
        return new VariableDeclarationNode();
    }
    ~VariableDeclarationNode() override = default;

    void SetVariable(ASTNode* variable) {m_variable = variable; variable->AddParent(this);}
    void SetKind (DeclarationKind kind) {m_kind = kind;}
    void SetInit (ASTNode* init) {m_init = init; init->AddParent(this);}
    bool isVariableDeclarationNode() override {return true;}


    nlohmann::json toJson() override {
        nlohmann::json jsonObj;
        jsonObj["type"] = "VariableDeclarator";
        jsonObj["kind"] = DeclarationKindToString(m_kind);
        jsonObj["id"] = m_variable->toJson();
        jsonObj["init"] = m_init->toJson();


        return jsonObj;
    }

private:
    static std::string DeclarationKindToString(DeclarationKind kind) {
        switch (kind) {
            case DeclarationKind::LET: return "let";
            case DeclarationKind::VAR: return "var";
            case DeclarationKind::CONST: return "const";
            default: return "invalid";
        }
    }
private:
    VariableDeclarationNode() = default;
    ASTNode* m_variable {nullptr};
    DeclarationKind m_kind {DeclarationKind::INVALID};
    ASTNode* m_init {nullptr};
};



class BinaryOpNode : public ASTNode{
public:
    static BinaryOpNode* Create() {
        return new BinaryOpNode();
    }

    ~BinaryOpNode() override = default;

    bool isBinaryOpNode() override {return true;}

    void SetSubType(BinaryOpSubType OpType) {m_SubType = OpType;}

    Interpreter::GenerateBytecodeResult GenerateBytecode (Interpreter::BytecodeGenerator& generator) override {
        generator.BuildCommand(Interpreter::BytecodeGenerator::OpCode::LOAD_STORE, Interpreter::BytecodeGenerator::OpCodeType::LOAD, 0xFF, 0x2);
        return Interpreter::GenerateBytecodeResult::SUCCESS;
    }
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
    void RemoveLhs() override {LhsNode = nullptr; LhsNode->RemoveParent();}
    void RemoveRhs() override {RhsNode = nullptr; RhsNode->RemoveParent();}
    void SetLhs(ASTNode* lhs) override {LhsNode = lhs; lhs->AddParent(this);}
    void SetRhs(ASTNode* rhs) override {RhsNode = rhs; rhs->AddParent(this);}
    ASTNode* Lhs() override {return LhsNode;}
    ASTNode* Rhs() override {return RhsNode;}

private:
    BinaryOpNode() = default;

    BinaryOpSubType m_SubType;
    ASTNode* LhsNode {nullptr};
    ASTNode* RhsNode {nullptr};

};
    class BreakNode : public ASTNode {
    private:
        BreakNode() = default;

    public:
        static BreakNode* Create() {
            return new BreakNode();
        }
        ~BreakNode() override = default;
        void SetLabel(std::string&& label) {m_label = std::move(label);}
        void SetLabel(const std::string& label) {m_label = label;}
        nlohmann::json toJson() override {
            nlohmann::json jsonObj;
            jsonObj["NodeType"] = "break";
            jsonObj["label"] = m_label;
            return jsonObj;
        }
    private:
        std::string m_label;
    };
    class ContinueNode : public ASTNode {};

class IfNode : public ASTNode {
public:
    static IfNode* Create() {
        return new IfNode();
    }

    void SetAlternatePresent() {isAlternatePresent = true;}
    void SetTestNode(ASTNode* TestNode) override {Condition = TestNode;}
    void SetConsequentNode(ASTNode* ConsequentNode) override {Consequent = ConsequentNode;}
    void SetAlternateNode(ASTNode* AlternateNode) override {Alternate = AlternateNode;}

    ASTNode* TestNode() const {return Condition;}
    ASTNode* ConsequentNode() const {return Consequent;}
    ASTNode* AlternateNode() const {return Alternate;}

    nlohmann::json toJson() override {
        nlohmann::json jsonObj;
        jsonObj["type"] = "IfStatement";
        jsonObj["condition"] = Condition->toJson();
        jsonObj["consequent"] = Consequent->toJson();
        if (isAlternatePresent)
            jsonObj["alternate"] = Alternate->toJson();
        return jsonObj;
    }

    bool isIfNode() override {return true;}

    ~IfNode() override = default;
private:
    IfNode() = default;
private:
    ASTNode* Condition {nullptr};
    ASTNode* Consequent {nullptr};
    ASTNode* Alternate {nullptr};

    bool isAlternatePresent {false};

};
class ForNode : public ASTNode {
public:
    static ForNode* Create() {
        return new ForNode();
    }

    void SetInitExpr(ASTNode* init) {m_init = init;}
    void SetTestExpr(ASTNode* test) {m_test = test;}
    void SetUpdateExpr(ASTNode* update) {m_update = update;}
    void SetConsequentNode(ASTNode* consequent) override {m_consequent = consequent;}

    nlohmann::json toJson() override {
        nlohmann::json jsonObj;
        jsonObj["NodeType"] = "ForStatement";
        jsonObj["init"] = m_init->toJson();
        jsonObj["test"] = m_test->toJson();
        jsonObj["update"] = m_update->toJson();
        jsonObj["consequent"] = m_consequent->toJson();
        return jsonObj;
    }
private:
    ForNode() = default;
    ~ForNode() override = default;

    ASTNode* m_init {nullptr};
    ASTNode* m_test {nullptr};
    ASTNode* m_update {nullptr};
    ASTNode* m_consequent {nullptr};

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

    class ReturnNode : public ASTNode {
    private:
        ReturnNode() = default;
    public:
        ~ReturnNode() = default;
        static ReturnNode* Create() {
            return new ReturnNode();
        }
        nlohmann::json toJson() override {
            nlohmann::json jsonObj;
            jsonObj["raw"] = Raw();
            if (m_expr)
                jsonObj["returned_expression"] = m_expr->toJson();
            return jsonObj;
        }
        bool isReturnNode() override {return true;}
        void SetConsequentNode(ASTNode* expr) override {m_expr = expr;}
    private:
        ASTNode* m_expr;
    };

    class FunctionDeclaration : public ASTNode {
    public:
        static FunctionDeclaration* Create() {
            return new FunctionDeclaration();
        }
        ~FunctionDeclaration() override = default;

        void AddArgument(ASTNode* arg) {
            m_args.push_back(arg);
            arg->AddParent(this);
        }

        void SetBody(ASTNode* body) {
            m_body = body;
            body->AddParent(this);
        }

        void SetAsync() {
            m_isAsync = true;
        }
        void SetGenerator() {
            m_isGenerator = true;
        }
        void SetExpression() {
            m_isExpression = true;
        }

        Interpreter::GenerateBytecodeResult GenerateBytecode(Interpreter::BytecodeGenerator& generator) override {
            return Interpreter::GenerateBytecodeResult::UNSPECIFIED_NODE;

        }
    private:
        FunctionDeclaration() = default;
        std::vector<ASTNode*> m_args;
        ASTNode* m_body;
        bool m_isAsync {false};
        bool m_isGenerator {false};
        bool m_isExpression {false};


        bool isFunctionDeclaration() override {return true;}

        nlohmann::json toJson() override {
            nlohmann::json jsonObj;
            jsonObj["NodeType"] = "FunctionDeclaration";
            jsonObj["isAsync"] = m_isAsync;
            jsonObj["isGenerator"] = m_isGenerator;
            jsonObj["isExpression"] = m_isExpression;
            jsonObj["arguments"] = nlohmann::json::array();
            for (const auto& arg : m_args) {
                jsonObj["arguments"].push_back(arg->toJson());
            }
            if (m_body) {
                jsonObj["body"] = m_body->toJson();
            } else {
                VERIFY_NOT_REACHED();
            }
            return jsonObj;
        }

    };

    class ExpressionStatement: public ASTNode {
    public:
        static ExpressionStatement* Create() {
            return new ExpressionStatement();
        }
        ~ExpressionStatement() override = default;

        void SetExpression(ASTNode* expression) {
            m_expression = expression;
            expression->AddParent(this);
        }

        ASTNode* Expression() {return m_expression;}

        nlohmann::json toJson() override {
            nlohmann::json jsonObj;
            jsonObj["NodeType"] = "ExpressionStatement";
            jsonObj["expression"] = m_expression->toJson();
            return jsonObj;
        }
        ASTNode* m_expression {nullptr};
        bool isExpressionStatement() {return true;}
    private:
        ExpressionStatement() = default;
    };

    class CallExpression : public ASTNode {
        public:
        static CallExpression* Create() {
            return new CallExpression();
        }
        ~CallExpression() override = default;

        private:
        CallExpression() = default;

        std::vector<VariableNode*> m_args;
    };


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
        ASTNode* m_CurrentNode {};
        ASTNode* m_LastUsedMainBranchNode {};
        Stack<Token> m_OperatorStack;
    };


}




