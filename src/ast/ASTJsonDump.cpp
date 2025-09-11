#include <nlohmann/json.hpp>
#include "AST.h"

namespace js::AST {
    nlohmann::json ASTNode::toJson() {
        nlohmann::json jsonObj;
        jsonObj["raw"] = m_raw;
        jsonObj["NodeType"] = "ASTNode";
        return jsonObj;
    }
    nlohmann::json ProgramNode::toJson() {
        nlohmann::json jsonObj;
        jsonObj["NodeType"] = "Program";
        for (const auto& child : m_children) {
            jsonObj["children"].push_back(child->toJson());
        }
        return jsonObj;
    }
    nlohmann::json BlockNode::toJson() {
        nlohmann::json jsonObj;
        jsonObj["NodeType"] = "BlockStatement";
        for (const auto& child : m_children) {
            jsonObj["children"].push_back(child->toJson());
        }
        return jsonObj;
    }
    nlohmann::json Identifier::toJson() {
        nlohmann::json jsonObj;
        jsonObj["raw"] = m_raw;
        jsonObj["NodeType"] = "Value";
        return jsonObj;
    }

    nlohmann::json Literal::toJson() {
        nlohmann::json jsonObj;
        jsonObj["raw"] = Raw();
        jsonObj["numericValue"] = m_NumericValue;
        return jsonObj;
    }

    nlohmann::json AssignmentExpression::toJson() {
        nlohmann::json jsonObj;
        jsonObj["raw"] = Raw();
        jsonObj["lvalue"] = m_lvalue->toJson();
        jsonObj["rvalue"] = m_rvalue->toJson();
        return jsonObj;
    }

    nlohmann::json VariableDeclaration::toJson() {
        nlohmann::json jsonObj;
        jsonObj["type"] = "VariableDeclarator";
        jsonObj["kind"] = DeclarationKindToString(m_kind);
        jsonObj["id"] = m_variable->toJson();
        jsonObj["init"] = m_init->toJson();


        return jsonObj;
    }

    nlohmann::json BinaryOperation::toJson() {
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

    nlohmann::json BreakStatement::toJson() {
        nlohmann::json jsonObj;
        jsonObj["NodeType"] = "break";
        jsonObj["label"] = m_label;
        return jsonObj;
    }

    nlohmann::json IfStatement::toJson() {
        nlohmann::json jsonObj;
        jsonObj["type"] = "IfStatement";
        jsonObj["condition"] = Condition->toJson();
        jsonObj["consequent"] = Consequent->toJson();
        if (isAlternatePresent)
            jsonObj["alternate"] = Alternate->toJson();
        return jsonObj;
    }

    nlohmann::json ForStatement::toJson() {
        nlohmann::json jsonObj;
        jsonObj["NodeType"] = "ForStatement";
        jsonObj["init"] = m_init->toJson();
        jsonObj["test"] = m_test->toJson();
        jsonObj["update"] = m_update->toJson();
        jsonObj["consequent"] = m_consequent->toJson();
        return jsonObj;
    }

    nlohmann::json ReturnStatement::toJson() {
        nlohmann::json jsonObj;
        jsonObj["raw"] = Raw();
        if (m_expr)
            jsonObj["returned_expression"] = m_expr->toJson();
        return jsonObj;
    }

    nlohmann::json FunctionDeclaration::toJson() {
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

    nlohmann::json ExpressionStatement::toJson() {
        nlohmann::json jsonObj;
        jsonObj["NodeType"] = "ExpressionStatement";
        jsonObj["expression"] = m_expression->toJson();
        return jsonObj;
    }

}