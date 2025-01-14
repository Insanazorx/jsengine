#pragma once
#include "StdLib.h"
#include "EnumTypes.h"
#include <string>

namespace JSLib {
    enum class TokenType{
        IF, FOR, DO,
        WHILE, BINARY_OP,
        OF, IN, GET, SET, FALSE,
        NEW, DELETE, THROW, TRUE,
        TRY, CATCH,
        EVAL, EXTENDS, ARGUMENTS,
        BREAK, CONTINUE, SUPER, CONST,
        SWITCH, THIS, IMPORT, FROM,
        AS,UNARY_OP, VAR,LET, FUNCTION,
        ELSE, L_BRACE, STRING,
        END_OF_STREAM, SEMICOLON, COMMA,
        DOT, AWAIT, ASYNC, GENERATOR_FUNCTION,
        R_BRACE, L_PARENTHESES, R_PARENTHESES,
        RETURN, CLASS, NUMERIC,
        IDENTIFIER, INVALID, END_OF_STATEMENT,
        START_OF_SCOPE, END_OF_SCOPE
    };

    struct Token {
        Token() = default;
        ~Token() = default;

        TokenType Type;
        std::string Lexeme;
        bool Analyzed {false};
        bool isSign {false};

        Token(const Token& other) {
            Type = other.Type;
            Lexeme = other.Lexeme;
            Analyzed = other.Analyzed;
            isSign = other.isSign;
        }
        void operator=(const Token& other) {
            Type = other.Type;
            Lexeme = other.Lexeme;
            Analyzed = other.Analyzed;
            isSign = other.isSign;
        }
    };
}