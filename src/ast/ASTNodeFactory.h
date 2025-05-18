#include "AST.h"


namespace js {
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
                auto node = VariableNode::Create();
                node->SetValue(ast_json["name"]);
                return node;
            }
            if (type == "Literal") {
                auto node = ValueNode::Create();

                node->SetValue(ast_json["raw"]);

                return node;
            }
            if (type == "BinaryExpression") {
                auto node = BinaryOpNode::Create();
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

            if (type == "VariableDeclaration") {
                auto node = VariableDeclarationNode::Create();
                auto kind_string = ast_json["kind"];
                if (kind_string == "let") {
                    node->SetKind(DeclarationKind::LET);
                } else if (kind_string == "const") {
                    node->SetKind(DeclarationKind::CONST);
                } else if (kind_string == "var") {
                    node->SetKind(DeclarationKind::VAR);
                }
                for (const auto& decl : ast_json["declarations"]) {
                    auto var_decl = VariableNode::Create();
                    var_decl->SetValue(decl["id"]["name"]);

                    node->SetVariable(var_decl);
                }
                std::cout << ast_json["declarations"][0]["init"].dump(4) << std::endl;
                node->SetInit(CreateFromJson(ast_json["declarations"][0]["init"]));
                return node;
            }

            if (type == "AssignmentExpression") {
                auto node = AssignmentNode::Create();
                node->SetValue(ast_json["operator"]);
                node->SetLvalue(CreateFromJson(ast_json["left"]));
                node->SetRvalue(CreateFromJson(ast_json["right"]));
                return node;
            }


            if (type == "IfStatement") {
                auto node = IfNode::Create();
                node->SetTestNode(CreateFromJson(ast_json["test"]));
                node->SetConsequentNode(CreateFromJson(ast_json["consequent"]));
                if (!ast_json["alternate"].is_null()) {
                    node->SetAlternatePresent();
                    node->SetAlternateNode(CreateFromJson(ast_json["alternate"]));
                }
                return node;
            }

            if (type == "WhileStatement") {
                auto node = WhileNode::Create();
                node->SetTestNode(CreateFromJson(ast_json["test"]));
                node->SetConsequentNode(CreateFromJson(ast_json["body"]));
                return node;
            }

            if (type == "ForStatement") {
                auto node = ForNode::Create();
                node->SetInitExpr(CreateFromJson(ast_json["init"]));
                node->SetTestExpr(CreateFromJson(ast_json["test"]));
                node->SetUpdateExpr(CreateFromJson(ast_json["update"]));
                node->SetConsequentNode(CreateFromJson(ast_json["body"]));
                return node;
            }

            if (type == "BreakStatement") {
                auto node = BreakNode::Create();
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