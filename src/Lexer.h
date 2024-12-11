
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
            TokenTypeMap["return"] = TokenType::RETURN;
            TokenTypeMap["function"] = TokenType::FUNCTION;
            TokenTypeMap["var"] = TokenType::VAR;
            TokenTypeMap["let"] = TokenType::LET;
            TokenTypeMap["if"] = TokenType::IF;
            TokenTypeMap["else"] = TokenType::ELSE;
            TokenTypeMap["for"] = TokenType::FOR;
            TokenTypeMap["while"] = TokenType::WHILE;
            TokenTypeMap["class"] = TokenType::CLASS;
            TokenTypeMap["("] = TokenType::L_PARENTHESES;
            TokenTypeMap[")"] = TokenType::R_PARENTHESES;
            TokenTypeMap["{"] = TokenType::L_BRACE;
            TokenTypeMap["}"] = TokenType::R_BRACE;
            TokenTypeMap[";"] = TokenType::END_OF_STATEMENT;
            TokenTypeMap[","] = TokenType::COMMA;
            TokenTypeMap[":"] = TokenType::SEMICOLON;
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
