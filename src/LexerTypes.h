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
        END_OF_STREAM, COLON, COMMA,
        DOT, AWAIT, ASYNC, GENERATOR_FUNCTION,
        R_BRACE, L_BRACKET, R_BRACKET,
        RETURN, CLASS, NUMERIC,
        IDENTIFIER, INVALID, SEMICOLON,
        STATEMENT_HOLDER
    };

    struct TokenPosition {
        int Line {0};
        int Pos {0};
    };

    struct Token {
        Token() = default;
        ~Token() = default;

        TokenType Type;
        std::string Lexeme;
        bool Analyzed {false};
        TokenPosition Position;

        Token(const Token& other) {
            Type = other.Type;
            Lexeme = other.Lexeme;
            Analyzed = other.Analyzed;
            Position = other.Position;
        }

        Token& operator=(const Token& other) = default;

    };
    struct StatementHolder : Token {
        StatementHolder() {
          Type = TokenType::STATEMENT_HOLDER;
        }
        ~StatementHolder() = default;

        StatementType allowed_types {StatementType::ANY_OF_STATEMENT_TYPES};

    };
}