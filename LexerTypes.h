#pragma once
#include "StdLib.h"
#include "StatementTypes.h"

namespace JSLib { 
using namespace Std;
enum class TokenType{
        IF, FOR, WHILE, BINARY_OP, VAR,LET, FUNCTION, ELSE, LBRACKET,
        RBRACKET, LPARENTHESS, RPARENTHESS, RETURN, CLASS, NUMERIC, IDENTIFIER, INVALID, END_OF_STATEMENT, START_OF_SCOPE, END_OF_SCOPE
    };

struct Token {
        TokenType Type;
        String Lexeme;
        bool Analyzed;
        Optional<StatementType> StatType;
    };
}