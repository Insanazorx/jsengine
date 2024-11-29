#include "Lexer.h"

#include <unordered_map>

#include "debug.h"
#include "SpecialHash.h"

using namespace JSLib;

void Lexer::Scan() {
    std::std::string chars("+ - * / = ; || << ( ) { } >> < > != == <= >= ===");
    std::vector<std::std::string> specials = chars.split(' ');
    std::vector<int> poses;

    for (int i = 0; i< m_Data.length(); i++) {
        for (auto special: specials) {
            if (m_Data.search_at_specific_index(i,special)) {
                poses.push_back(i);
                poses.push_back(i+special.length());
            }
        }
    }

    for (int j = 0; j < poses.getSize(); j++) {
        poses[j] += j;
    }

    for (auto pos: poses) m_Data.insert(pos, " ");

    auto results = m_Data.split(' ');

    for (auto res: results) DEBUG(res.c_str());
    

    for (auto lexeme: results) {
        auto newToken = *new Token;
        newToken.Lexeme = lexeme;
        IdentifyToken(lexeme,newToken);
        m_Tokens.push_back(newToken);
    }
}

void Lexer::IdentifyToken(std::std::string& lexemetoid, Token& tokentoid) {
        if (lexemetoid.isNumeric()) {
            tokentoid.Type = TokenType::NUMERIC;
            return;
        }
        std::unordered_map<std::string, TokenType, Util::std::stringHash> TokenTypeMap;

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

        auto LexemeType = TokenTypeMap.find(lexemetoid);
            if (LexemeType != TokenTypeMap.end()) {
                auto lexemetype = LexemeType->second;
                tokentoid.Type = lexemetype;
            } else {
                tokentoid.Type = TokenType::IDENTIFIER;
            }
            }

