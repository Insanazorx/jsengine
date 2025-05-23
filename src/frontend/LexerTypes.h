#pragma once
#include "StdLib.h"
#include "EnumTypes.h"
#include <string>

namespace js {
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
        STATEMENT_HOLDER, END_OF_STATEMENT, VOID_TOKEN
    };

    enum class StatementHolderType {
        TEST, CONSEQUENT,
        ALTERNATE, NOT_HOLDER,
        BODY, ARGUMENTS,
        UPDATE, INIT
    };

    struct TokenPosition {
        int Line {0};
        int Pos {0};
    };

    struct Token {
        Token() = default;
        virtual ~Token() = default;

        TokenType Type;
        std::string Lexeme;
        bool Analyzed {false};
        TokenPosition Position;

        StatementHolderType holder_type {StatementHolderType::NOT_HOLDER};

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

    struct EndOfStatement: Token {
        EndOfStatement() {
            Type = TokenType::END_OF_STATEMENT;
        }
        ~EndOfStatement() = default;
    };

    struct VoidToken : Token {
        VoidToken() {
            Type = TokenType::VOID_TOKEN;
        }
        ~VoidToken() = default;
    };
}