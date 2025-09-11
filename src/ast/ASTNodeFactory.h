#pragma once
#include "AST.h"


namespace js::AST {
    class AstNodeFactory {
    public:

        static ASTNode* CreateFromJson(const nlohmann::json& ast_json) {
            if (ast_json.is_null())
                throw std::runtime_error("JSON nesnesi bos.");

            if (!ast_json.contains("type"))
                throw std::runtime_error("JSON node da 'type' alani eksik.");

            std::string type = ast_json["type"];

            if (type == "Program") {
                auto node = ProgramNode::Create();
                for (const auto& child : ast_json["body"]) {
                    node->AddChild(CreateFromJson(child));
                }
                return node;
            }

            if (type == "Identifier") {
                auto node = Identifier::Create();
                node->SetValue(ast_json["name"]);
                return node;
            }
            if (type == "Literal") {
                auto node = Identifier::Create();

                node->SetValue(ast_json["raw"]);

                return node;
            }
            if (type == "BinaryExpression") {
                auto node = BinaryOperation::Create();
                node->SetValue(ast_json["operator"]);
                node->SetLhs(CreateFromJson(ast_json["left"]));
                node->SetRhs(CreateFromJson(ast_json["right"]));
                node->SetSubType(StringToBinaryOpSubType(ast_json["operator"]));
                return node;
            }

            if (type == "BlockStatement") {
                auto node = BlockNode::Create();
                for (const auto& child : ast_json["body"]) {
                    node->AddChild(CreateFromJson(child));
                }
                return node;
            }

            if (type == "ExpressionStatement") {
                auto node = ExpressionStatement::Create();
                node->SetExpression(CreateFromJson(ast_json["expression"]));
                return node;
            }

            if (type == "VariableDeclaration") {
                auto node = VariableDeclaration::Create();
                auto kind_string = ast_json["kind"];
                if (kind_string == "let") {
                    node->SetKind(DeclarationKind::LET);
                } else if (kind_string == "const") {
                    node->SetKind(DeclarationKind::CONST);
                } else if (kind_string == "var") {
                    node->SetKind(DeclarationKind::VAR);
                }
                for (const auto& decl : ast_json["declarations"]) {
                    auto var_decl = Identifier::Create();
                    var_decl->SetValue(decl["id"]["name"]);

                    node->SetVariable(var_decl);
                }
                std::cout << ast_json["declarations"][0]["init"].dump(4) << std::endl;
                node->SetInit(CreateFromJson(ast_json["declarations"][0]["init"]));
                return node;
            }

            if (type == "AssignmentExpression") {
                auto node = AssignmentExpression::Create();
                node->SetValue(ast_json["operator"]);
                node->SetLvalue(CreateFromJson(ast_json["left"]));
                node->SetRvalue(CreateFromJson(ast_json["right"]));
                return node;
            }


            if (type == "IfStatement") {
                auto node = IfStatement::Create();
                node->SetTestNode(CreateFromJson(ast_json["test"]));
                node->SetConsequentNode(CreateFromJson(ast_json["consequent"]));
                if (!ast_json["alternate"].is_null()) {
                    node->SetAlternatePresent();
                    node->SetAlternateNode(CreateFromJson(ast_json["alternate"]));
                }
                return node;
            }

            if (type == "WhileStatement") {
                auto node = WhileStatement::Create();
                node->SetTestNode(CreateFromJson(ast_json["test"]));
                node->SetConsequentNode(CreateFromJson(ast_json["body"]));
                return node;
            }

            if (type == "ForStatement") {
                auto node = ForStatement::Create();
                node->SetInitExpr(CreateFromJson(ast_json["init"]));
                node->SetTestExpr(CreateFromJson(ast_json["test"]));
                node->SetUpdateExpr(CreateFromJson(ast_json["update"]));
                node->SetConsequentNode(CreateFromJson(ast_json["body"]));
                return node;
            }

            if (type == "BreakStatement") {
                auto node = BreakStatement::Create();
                return node;
            }

            if (type == "FunctionDeclaration") {
                auto node = FunctionDeclaration::Create();
                for (const auto& arg : ast_json["params"]) {
                    auto arg_node = Identifier::Create();
                    arg_node->SetValue(arg["name"]);
                    node->AddArgument(arg_node);
                }

                bool isGenerator = ast_json.contains("generator") && ast_json["generator"].get<bool>();
                bool isAsync = ast_json.contains("async") && ast_json["async"].get<bool>();
                bool isExpression = ast_json.contains("expression") && ast_json["expression"].get<bool>();

                if (isGenerator) {
                    node->SetGenerator();
                }
                if (isAsync) {
                    node->SetAsync();
                }
                if (isExpression) {
                    node->SetExpression();
                }


                if (!ast_json["body"].is_null()) {
                    node->SetBody(CreateFromJson(ast_json["body"]));
                }

                return node;
            }

            if (type == "ReturnStatement") {
                auto node = ReturnStatement::Create();
                if (!ast_json["argument"].is_null()) {
                    node->SetConsequentNode(CreateFromJson(ast_json["argument"]));
                }
                return node;
            }

            std::cout<< "Unknown AST node type: " << type << std::endl;
            VERIFY_NOT_REACHED();
        }
    private:
        static BinaryOpSubType StringToBinaryOpSubType(const std::string& op) {
            if (op == "+") return BinaryOpSubType::ADD;
            if (op == "-") return BinaryOpSubType::SUB;
            if (op == "*") return BinaryOpSubType::MUL;
            if (op == "/") return BinaryOpSubType::DIV;
            if (op == "%") return BinaryOpSubType::MOD;
            if (op == "**") return BinaryOpSubType::POW;
            if (op == "<<") return BinaryOpSubType::SHIFT_LEFT;
            if (op == ">>") return BinaryOpSubType::SHIFT_RIGHT;
            if (op == "<") return BinaryOpSubType::LESS_THAN;
            if (op == "<=") return BinaryOpSubType::LESS_OR_EQUAL;
            if (op == ">") return BinaryOpSubType::GREATER_THAN;
            if (op == ">=") return BinaryOpSubType::GREATER_OR_EQUAL;
            if (op == "==") return BinaryOpSubType::IS_EQUAL;
            if (op == "!=") return BinaryOpSubType::IS_NOT_EQUAL;
            if (op == "===") return BinaryOpSubType::IS_STRICTLY_EQUAL;
            if (op == "!==") return BinaryOpSubType::IS_NOT_STRICTLY_EQUAL;
            if (op == "&&") return BinaryOpSubType::LOGICAL_AND;
            if (op == "||") return BinaryOpSubType::LOGICAL_OR;
            if (op == "&") return BinaryOpSubType::BITWISE_AND;
            if (op == "|") return BinaryOpSubType::BITWISE_OR;
            if (op == "^") return BinaryOpSubType::BITWISE_XOR;
            if (op == "instanceof") return BinaryOpSubType::INSTANCEOF;
            if (op == "in") return BinaryOpSubType::IN;

            throw std::runtime_error("Bilinmeyen operator: " + op);
        }
    };

}
