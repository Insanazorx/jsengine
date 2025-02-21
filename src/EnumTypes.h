
#pragma once
#include <string>

#include "debug.h"

namespace JSLib {
    enum class StatementType{
        PROGRAM, IF_STATEMENT, FOR_STATEMENT, RETURN_STATEMENT,
        WHILE_STATEMENT, SWITCH_CASE_STATEMENT,IMMEDIATE_STATEMENT,
        BINARY_OP_STATEMENT,VARIABLE_STATEMENT, UNARY_OP_STATEMENT,
        ASSIGNMENT_STATEMENT,FUNCTION_STATEMENT, CLASS_STATEMENT,
        OBJECT_STATEMENT, SCOPE_STATEMENT,
        VARIABLE_DECLARATION_STATEMENT, BRACKET_STATEMENT,
        ANY_OF_STATEMENT_TYPES,INVALID_STATEMENT
    };
    enum class BinaryOpSubType {
        POW,
        MUL,DIV,MOD,
        ADD,SUB,
        SHIFT_LEFT,SHIFT_RIGHT,
        IN,INSTANCEOF ,GREATER_THAN,LESS_THAN ,GREATER_OR_EQUAL ,LESS_OR_EQUAL,
        IS_EQUAL, IS_NOT_EQUAL, IS_STRICTLY_EQUAL, IS_NOT_STRICTLY_EQUAL,
        BITWISE_OR , BITWISE_XOR, BITWISE_AND,
        LOGICAL_OR, LOGICAL_AND,
        L_BRACKET, R_BRACKET
    };
    enum class ExpressionScaffoldingType {
        EXPR_WITH_BRACKETS_AND_OPTIONAL_BRACES, EXPR_WITH_NONE, EXPR_WITH_BRACKETS,
        EXPR_WITH_BRACKETS_AND_BRACES, IF_EXPR
    };


    inline std::string StringifyBinaryOpSubType(BinaryOpSubType type) {
        switch (type) {
            case BinaryOpSubType::POW:
                return "POW";
            case BinaryOpSubType::MUL:
                return "MUL";
            case BinaryOpSubType::DIV:
                return "DIV";
            case BinaryOpSubType::MOD:
                return "MOD";
            case BinaryOpSubType::ADD:
                return "ADD";
            case BinaryOpSubType::SUB:
                return "SUB";
            case BinaryOpSubType::SHIFT_LEFT:
                return "SHIFT_LEFT";
            case BinaryOpSubType::SHIFT_RIGHT:
                return "SHIFT_RIGHT";
            case BinaryOpSubType::IN:
                return "IN";
            case BinaryOpSubType::INSTANCEOF:
                return "INSTANCEOF";
            case BinaryOpSubType::GREATER_THAN:
                return "GREATER_THAN";
            case BinaryOpSubType::LESS_THAN:
                return "LESS_THAN";
            case BinaryOpSubType::GREATER_OR_EQUAL:
                return "GREATER_OR_EQUAL";
            case BinaryOpSubType::LESS_OR_EQUAL:
                return "LESS_OR_EQUAL";
            case BinaryOpSubType::IS_EQUAL:
                return "IS_EQUAL";
            case BinaryOpSubType::IS_NOT_EQUAL:
                return "IS_NOT_EQUAL";
            case BinaryOpSubType::IS_STRICTLY_EQUAL:
                return "IS_STRICTLY_EQUAL";
            case BinaryOpSubType::IS_NOT_STRICTLY_EQUAL:
                return "IS_NOT_STRICTLY_EQUAL";
            case BinaryOpSubType::BITWISE_OR:
                return "BITWISE_OR";
            case BinaryOpSubType::BITWISE_XOR:
                return "BITWISE_XOR";
            case BinaryOpSubType::BITWISE_AND:
                return "BITWISE_AND";
            case BinaryOpSubType::LOGICAL_OR:
                return "LOGICAL_OR";
            case BinaryOpSubType::LOGICAL_AND:
                return "LOGICAL_AND";
            case BinaryOpSubType::L_BRACKET:
                return "L_BRACKET";
            case BinaryOpSubType::R_BRACKET:
                return "R_BRACKET";
            default:
                VERIFY_NOT_REACHED();
        }
    }

    inline int GetPrecedence (BinaryOpSubType type) {
        switch (type) {
            case BinaryOpSubType::POW:
                return 9;
            case BinaryOpSubType::MUL:
            case BinaryOpSubType::DIV:
            case BinaryOpSubType::MOD:
                return 8;
            case BinaryOpSubType::ADD:
            case BinaryOpSubType::SUB:
                return 7;
            case BinaryOpSubType::SHIFT_LEFT:
            case BinaryOpSubType::SHIFT_RIGHT:
                return 6;
            case BinaryOpSubType::IN:
                return 5;
            case BinaryOpSubType::INSTANCEOF:
            case BinaryOpSubType::GREATER_THAN:
            case BinaryOpSubType::LESS_THAN:
            case BinaryOpSubType::GREATER_OR_EQUAL:
            case BinaryOpSubType::LESS_OR_EQUAL:
                return 5;
            case BinaryOpSubType::IS_EQUAL:
            case BinaryOpSubType::IS_NOT_EQUAL:
            case BinaryOpSubType::IS_STRICTLY_EQUAL:
            case BinaryOpSubType::IS_NOT_STRICTLY_EQUAL:
                return 4;
            case BinaryOpSubType::BITWISE_OR:
            case BinaryOpSubType::BITWISE_XOR:
            case BinaryOpSubType::BITWISE_AND:
                return 3;
            case BinaryOpSubType::LOGICAL_OR:
            case BinaryOpSubType::LOGICAL_AND:
                return 2;
            case BinaryOpSubType::L_BRACKET:
            case BinaryOpSubType::R_BRACKET:
                return 0xFFFF;
            default:
                VERIFY_NOT_REACHED();
        }
    }

    enum class UnaryOpSubType {
        INC, DEC, BITWISE_NOT, LOGICAL_NOT,
    };
}
