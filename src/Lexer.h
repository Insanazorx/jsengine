
#pragma once
#include "StdLib.h"
#include "LexerTypes.h"
#include <vector>
#include <string>
#include <unordered_map>

namespace JSLib {

    class Lexer {
    public:
        Lexer()=default;
        Lexer(std::string data) : m_Data(data) {
            TokenTypeMap["+"] = TokenType::BINARY_OP;
            TokenTypeMap["-"] = TokenType::BINARY_OP;
            TokenTypeMap["*"] = TokenType::BINARY_OP;
            TokenTypeMap["/"] = TokenType::BINARY_OP;
            TokenTypeMap["%"] = TokenType::BINARY_OP;
            TokenTypeMap["<"] = TokenType::BINARY_OP;
            TokenTypeMap[">"] = TokenType::BINARY_OP;
            TokenTypeMap["|"] = TokenType::BINARY_OP;
            TokenTypeMap["&"] = TokenType::BINARY_OP;
            TokenTypeMap["^"] = TokenType::BINARY_OP;
            TokenTypeMap["||"] = TokenType::BINARY_OP;
            TokenTypeMap["&&"] = TokenType::BINARY_OP;
            TokenTypeMap["=="] = TokenType::BINARY_OP;
            TokenTypeMap["==="] = TokenType::BINARY_OP;
            TokenTypeMap["!="] = TokenType::BINARY_OP;
            TokenTypeMap["<="] = TokenType::BINARY_OP;
            TokenTypeMap[">="] = TokenType::BINARY_OP;
            TokenTypeMap["+="] = TokenType::BINARY_OP;
            TokenTypeMap["-="] = TokenType::BINARY_OP;
            TokenTypeMap["*="] = TokenType::BINARY_OP;
            TokenTypeMap["/="] = TokenType::BINARY_OP;
            TokenTypeMap["%="] = TokenType::BINARY_OP;
            TokenTypeMap["|="] = TokenType::BINARY_OP;
            TokenTypeMap["&="] = TokenType::BINARY_OP;
            TokenTypeMap["^="] = TokenType::BINARY_OP;
            TokenTypeMap["<<="] = TokenType::BINARY_OP;
            TokenTypeMap[">>="] = TokenType::BINARY_OP;
            TokenTypeMap["~"] = TokenType::UNARY_OP;
            TokenTypeMap["!"] = TokenType::UNARY_OP;
            TokenTypeMap["++"] = TokenType::UNARY_OP;
            TokenTypeMap["--"] = TokenType::UNARY_OP;
            TokenTypeMap["eval"] = TokenType::EVAL;
            TokenTypeMap["import"] = TokenType::IMPORT;
            TokenTypeMap["from"] = TokenType::FROM;
            TokenTypeMap["as"] = TokenType::AS;
            TokenTypeMap["instanceof"] = TokenType::BINARY_OP;
            TokenTypeMap["typeof"] = TokenType::UNARY_OP;
            TokenTypeMap["do"] = TokenType::DO;
            TokenTypeMap["true"] = TokenType::TRUE;
            TokenTypeMap["false"] = TokenType::FALSE;
            TokenTypeMap["new"] = TokenType::NEW;
            TokenTypeMap["delete"] = TokenType::DELETE;
            TokenTypeMap["try"] = TokenType::TRY;
            TokenTypeMap["catch"] = TokenType::CATCH;
            TokenTypeMap["throw"] = TokenType::THROW;
            TokenTypeMap["const"] = TokenType::CONST;
            TokenTypeMap["arguments"] = TokenType::ARGUMENTS;
            TokenTypeMap["break"] = TokenType::BREAK;
            TokenTypeMap["continue"] = TokenType::CONTINUE;
            TokenTypeMap["extends"] = TokenType::EXTENDS;
            TokenTypeMap["return"] = TokenType::RETURN;
            TokenTypeMap["super"] = TokenType::SUPER;
            TokenTypeMap["switch"] = TokenType::SWITCH;
            TokenTypeMap["this"] = TokenType::THIS;
            TokenTypeMap["function"] = TokenType::FUNCTION;
            TokenTypeMap["var"] = TokenType::VAR;
            TokenTypeMap["let"] = TokenType::LET;
            TokenTypeMap["if"] = TokenType::IF;
            TokenTypeMap["else"] = TokenType::ELSE;
            TokenTypeMap["for"] = TokenType::FOR;
            TokenTypeMap["while"] = TokenType::WHILE;
            TokenTypeMap["class"] = TokenType::CLASS;
            TokenTypeMap["await"] = TokenType::AWAIT;
            TokenTypeMap["async"] = TokenType::ASYNC;
            TokenTypeMap["function*"] = TokenType::GENERATOR_FUNCTION;
            TokenTypeMap["of"] = TokenType::OF;
            TokenTypeMap["in"] = TokenType::IN;
            TokenTypeMap["("] = TokenType::L_BRACKET;
            TokenTypeMap[")"] = TokenType::R_BRACKET;
            TokenTypeMap["{"] = TokenType::L_BRACE;
            TokenTypeMap["}"] = TokenType::R_BRACE;
            TokenTypeMap[";"] = TokenType::SEMICOLON;
            TokenTypeMap[","] = TokenType::COMMA;
            TokenTypeMap[":"] = TokenType::COLON;
            TokenTypeMap["."] = TokenType::DOT;
        } ;

        void SetExpr(std::string data) {m_Data = data;};

        void Scan();
        void IdentifyToken(std::string& lexemetoid, Token& tokentoid);
        std::vector<Token>& Tokens() {return m_Tokens;}
        static std::string StringifyTokenType (const Token& token) ;
        static std::string StringifyTokenType (TokenType token);


    private:
        std::string m_Data;
        std::vector<Token> m_Tokens;
        std::unordered_map<std::string, TokenType> TokenTypeMap;
    };

}
