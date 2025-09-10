

#pragma once

#include <random>
#include <nlohmann/json.hpp>
#include "../frontend/LexerTypes.h"
#include "../frontend/EnumTypes.h"
#include "../interpreter/Bytecode/BytecodeGenerator.h"


namespace js::AST {
    class Validator;
    class ASTVisitor;

    class StatementConverter;
class NodeBranchInfo;
class ASTBuilder;
class ASTNode;
class ParserContext;
class Parser;
    class Node;
#define BYTECODE_GENERATOR_HOOK \
        Interpreter::GenerateBytecodeResult GenerateBytecode(Interpreter::BytecodeGenerator&) override;

#define DECLARE_BYTECODE_GENERATOR_UNIT(name) \
    Interpreter::GenerateBytecodeResult name::GenerateBytecode(Interpreter::BytecodeGenerator& generator)

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
        virtual nlohmann::json toJson();


        virtual TokenPosition Validate(Validator& validator) {return {-1,-1};}

        virtual void SetTestNode(ASTNode* testNode) {}
        virtual void SetConsequentNode(ASTNode* consequentNode) {}
        virtual void SetAlternateNode(ASTNode* alternateNode) {}
        virtual ASTNode* Lhs() {}
        virtual ASTNode* Rhs() {}
        virtual void RemoveLhs() {}
        virtual void RemoveRhs() {}
        virtual void SetLhs(ASTNode* lhs) {};//for BinaryOperations
        virtual void SetRhs(ASTNode* rhs) {};//for BinaryOperations
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

        virtual Interpreter::GenerateBytecodeResult GenerateBytecode(Interpreter::BytecodeGenerator&);

        virtual bool isVariableNode() {return false;}
        virtual bool isLiteral() {return false;}
        virtual bool isBlockNode() {return false;}
        virtual bool isBinaryOperation() {return false;}
        virtual bool isUnaryOperation() {return false;}
        virtual bool isAssignmentExpression() {return false;}
        virtual bool isVariableDeclaration() {return false;}
        virtual bool isIfStatement() {return false;}
        virtual bool isWhileStatement() {return false;}
        virtual bool isForStatement() {return false;}
        virtual bool isForInNode() {return false;}
        virtual bool isForOfNode() {return false;}
        virtual bool isForAwaitOfNode() {return false;}
        virtual bool isFunctionDeclaration() {return false;}
        virtual bool isCallExpression() {return false;}
        virtual bool isArrowFunctionNode() {return false;}
        virtual bool isAsyncFunctionNode() {return false;}
        virtual bool isGeneratorFunctionNode() {return false;}
        virtual bool isReturnStatement() {return false;}


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

    nlohmann::json toJson() override;
    BYTECODE_GENERATOR_HOOK

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

    nlohmann::json toJson() override;
    BYTECODE_GENERATOR_HOOK

private:
    std::vector<ASTNode*> m_children;
    BlockNode() = default;

};
    class Identifier: public ASTNode {
    private:
        Identifier() = default;
    public:
        static Identifier* Create() {
            return new Identifier();
        }
        ~Identifier() override = default;
        void SetValue(std::string&& raw) override {m_raw = raw;}
        void SetValue(const std::string& raw) override{m_raw = raw;}
        bool isIdentifier() {return true;}

        nlohmann::json toJson() override;
        BYTECODE_GENERATOR_HOOK

    private:
        std::string m_raw;
    };


class Literal : public ASTNode {
private:
    Literal() = default;
public:
    static Literal* Create() {
        return new Literal();
    }
    ~Literal() override = default;
    void SetNumericValueFromRaw() override {
        m_NumericValue = std::stoi(Raw());
    }
    void SetNumericValue(int num) {m_NumericValue = num;}
    int NumericValue() const  {return m_NumericValue;}
    bool isLiteral() override {return true;}

    nlohmann::json toJson() override;
    BYTECODE_GENERATOR_HOOK

private:
    int m_NumericValue {0};
};



class UnaryOperation : public ASTNode {};
class AssignmentExpression : public ASTNode {
private:
    AssignmentExpression() = default;
public:
    static AssignmentExpression* Create() {
        return new AssignmentExpression();
    }
    ~AssignmentExpression() override = default;
    bool isAssignmentExpression() override {return true;}

    nlohmann::json toJson() override;
    BYTECODE_GENERATOR_HOOK

    void SetLvalue(ASTNode* lvalue) {m_lvalue = lvalue; lvalue->AddParent(this);}
    void SetRvalue(ASTNode* rvalue) {m_rvalue = rvalue; rvalue->AddParent(this);}

private:
    ASTNode* m_lvalue {nullptr};
    ASTNode* m_rvalue {nullptr};
};

    enum class DeclarationKind {
        CONST, VAR, LET, INVALID
    };
class VariableDeclaration : public ASTNode {
public:
    static VariableDeclaration* Create() {
        return new VariableDeclaration();
    }
    ~VariableDeclaration() override = default;

    void SetVariable(ASTNode* variable) {m_variable = variable; variable->AddParent(this);}
    void SetKind (DeclarationKind kind) {m_kind = kind;}
    void SetInit (ASTNode* init) {m_init = init; init->AddParent(this);}
    bool isVariableDeclaration() override {return true;}

    nlohmann::json toJson() override;
    BYTECODE_GENERATOR_HOOK


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
    VariableDeclaration() = default;
    ASTNode* m_variable {nullptr};
    DeclarationKind m_kind {DeclarationKind::INVALID};
    ASTNode* m_init {nullptr};
};



class BinaryOperation : public ASTNode{
public:
    static BinaryOperation* Create() {
        return new BinaryOperation();
    }

    ~BinaryOperation() override = default;

    bool isBinaryOperation() override {return true;}

    void SetSubType(BinaryOpSubType OpType) {m_SubType = OpType;}

    BYTECODE_GENERATOR_HOOK

    nlohmann::json toJson() override;

    BinaryOpSubType SubType() {return m_SubType;}
    void RemoveLhs() override {LhsNode = nullptr; LhsNode->RemoveParent();}
    void RemoveRhs() override {RhsNode = nullptr; RhsNode->RemoveParent();}
    void SetLhs(ASTNode* lhs) override {LhsNode = lhs; lhs->AddParent(this);}
    void SetRhs(ASTNode* rhs) override {RhsNode = rhs; rhs->AddParent(this);}
    ASTNode* Lhs() override {return LhsNode;}
    ASTNode* Rhs() override {return RhsNode;}

private:

    BinaryOperation() = default;

    BinaryOpSubType m_SubType;
    ASTNode* LhsNode {nullptr};
    ASTNode* RhsNode {nullptr};

};
    class BreakStatement : public ASTNode {
    private:
        BreakStatement() = default;

    public:
        static BreakStatement* Create() {
            return new BreakStatement();
        }
        ~BreakStatement() override = default;
        void SetLabel(std::string&& label) {m_label = std::move(label);}
        void SetLabel(const std::string& label) {m_label = label;}

        nlohmann::json toJson() override;
        BYTECODE_GENERATOR_HOOK

    private:
        std::string m_label;
    };
    class ContinueStatement : public ASTNode {};

class IfStatement : public ASTNode {
public:
    static IfStatement* Create() {
        return new IfStatement();
    }

    void SetAlternatePresent() {isAlternatePresent = true;}
    void SetTestNode(ASTNode* TestNode) override {Condition = TestNode;}
    void SetConsequentNode(ASTNode* ConsequentNode) override {Consequent = ConsequentNode;}
    void SetAlternateNode(ASTNode* AlternateNode) override {Alternate = AlternateNode;}

    ASTNode* TestNode() const {return Condition;}
    ASTNode* ConsequentNode() const {return Consequent;}
    ASTNode* AlternateNode() const {return Alternate;}

    nlohmann::json toJson() override;
    BYTECODE_GENERATOR_HOOK

    bool isIfStatement() override {return true;}

    ~IfStatement() override = default;
private:
    IfStatement() = default;
private:
    ASTNode* Condition {nullptr};
    ASTNode* Consequent {nullptr};
    ASTNode* Alternate {nullptr};

    bool isAlternatePresent {false};

};
class ForStatement : public ASTNode {
public:
    static ForStatement* Create() {
        return new ForStatement();
    }

    void SetInitExpr(ASTNode* init) {m_init = init;}
    void SetTestExpr(ASTNode* test) {m_test = test;}
    void SetUpdateExpr(ASTNode* update) {m_update = update;}
    void SetConsequentNode(ASTNode* consequent) override {m_consequent = consequent;}

    nlohmann::json toJson() override;
    BYTECODE_GENERATOR_HOOK

private:
    ForStatement() = default;
    ~ForStatement() override = default;

    ASTNode* m_init {nullptr};
    ASTNode* m_test {nullptr};
    ASTNode* m_update {nullptr};
    ASTNode* m_consequent {nullptr};

};
class WhileStatement : public ASTNode {
public:
    static WhileStatement* Create() {
        return new WhileStatement();
    }

    BYTECODE_GENERATOR_HOOK
private:
    WhileStatement() = default;
    ~WhileStatement() = default;
};

    class ReturnStatement : public ASTNode {
    private:
        ReturnStatement() = default;
    public:
        ~ReturnStatement() = default;
        static ReturnStatement* Create() {
            return new ReturnStatement();
        }

        nlohmann::json toJson() override;
        BYTECODE_GENERATOR_HOOK

        bool isReturnStatement() override {return true;}
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
        bool IsAsync() const {return m_isAsync;}
        bool IsGenerator() const {return m_isGenerator;}
        bool IsExpression() const {return m_isExpression;}

        nlohmann::json toJson() override;
        BYTECODE_GENERATOR_HOOK

    private:
        FunctionDeclaration() = default;
        std::vector<ASTNode*> m_args;
        ASTNode* m_body;
        bool m_isAsync {false};
        bool m_isGenerator {false};
        bool m_isExpression {false};


        bool isFunctionDeclaration() override {return true;}

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

        nlohmann::json toJson() override;

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

        nlohmann::json toJson() override;
        BYTECODE_GENERATOR_HOOK

        bool isCallExpression() override {return true;}

        private:
        CallExpression() = default;

        std::vector<Identifier*> m_args;
    };

}




