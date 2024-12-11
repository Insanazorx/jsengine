#pragma once
#include "StdLib.h"
#include "EnumTypes.h"
#include <string>

namespace JSLib {
    enum class TokenType{
        IF, FOR, WHILE, BINARY_OP, UNARY_OP, VAR,LET, FUNCTION, ELSE, L_BRACE, STRING, END_OF_STREAM, SEMICOLON, COMMA, DOT,
        R_BRACE, L_PARENTHESES, R_PARENTHESES, RETURN, CLASS, NUMERIC, IDENTIFIER, INVALID, END_OF_STATEMENT, START_OF_SCOPE, END_OF_SCOPE
    };

    struct Token {
        Token() = default;
        ~Token() = default;


        TokenType Type;
        std::string Lexeme;
        bool Analyzed {false};
        StatementType StatType {StatementType::PROGRAM};
        Token(const Token& other) {
            Type = other.Type;
            Lexeme = other.Lexeme;
            Analyzed = other.Analyzed;
            StatType = other.StatType;
        }
        void operator=(const Token& other) {
            Type = other.Type;
            Lexeme = other.Lexeme;
            Analyzed = other.Analyzed;
            StatType = other.StatType;
        }
    };
}