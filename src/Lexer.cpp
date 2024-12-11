#include "Lexer.h"

#include <unordered_map>
#include <vector>
#include <string>

#include "Util.h"
#include "debug.h"

using namespace JSLib;

void Lexer::Scan() {
    const std::string chars("+ - * / = ; || << ( ) { } >> < > != == <= >= ===");
    std::vector<std::string> specials = Split(chars, ' ');
    std::vector<int> poses;

    for (int i = 0; i< m_Data.length(); i++) {
        for (auto special: specials) {
            if (SearchAtSpecificIndex(m_Data,i,special)) {
                poses.push_back(i);
                poses.push_back(i+special.length());
            }
        }
    }

    for (int j = 0; j < poses.size(); j++) {
        poses[j] += j;
    }

    for (auto pos: poses) m_Data.insert(pos, " ");

    auto results = Split(m_Data, ' ');

    for (auto lexeme: results) {
        DEBUG(GREEN_TEXT <<"LEXER " << FUNCTION_NAME()<< RESET_TEXT << ": "<< lexeme);
        Token newToken = *new Token();
        newToken.Lexeme = lexeme;
        IdentifyToken(lexeme,newToken);
        m_Tokens.push_back(newToken);
    }
    auto eosToken = *new Token();
    eosToken.Type= TokenType::END_OF_STREAM;
    m_Tokens.push_back(eosToken);
}

std::string Lexer::StringifyTokenType (const Token& token) {
    switch (token.Type) {
        case TokenType::BINARY_OP:
            return "BINARY_OP";
        case TokenType::UNARY_OP:
            return "UNARY_OP";
        case TokenType::IDENTIFIER:
            return "IDENTIFIER";
        case TokenType::STRING:
            return "STRING";
        case TokenType::IF:
            return "IF";
        case TokenType::ELSE:
            return "ELSE";
        case TokenType::WHILE:
            return "WHILE";
        case TokenType::RETURN:
            return "RETURN";
        case TokenType::FOR:
            return "FOR";
        case TokenType::LET:
            return "LET";
        case TokenType::VAR:
            return "VAR";
        case TokenType::CLASS:
            return "CLASS";
        case TokenType::L_BRACE:
            return "L_BRACE";
        case TokenType::R_BRACE:
            return "R_BRACE";
        case TokenType::INVALID:
            return "INVALID";
        case TokenType::NUMERIC:
            return "NUMERIC";
        case TokenType::FUNCTION:
            return "FUNCTION";
        case TokenType::L_PARENTHESES:
            return "L_PARENTHESES";
        case TokenType::R_PARENTHESES:
            return "R_PARENTHESES";
        case TokenType::END_OF_STATEMENT:
            return "END_OF_STATEMENT";
        case TokenType::END_OF_STREAM:
            return "END_OF_STREAM";
        case TokenType::COMMA:
            return "COMMA";
        case TokenType::SEMICOLON:
            return "SEMICOLON";
        case TokenType::DOT:
            return "DOT";
        default:
            //VERIFY_NOT_REACHED();
            return "INVALID";
        break;
    }
}

std::string Lexer::StringifyTokenType (TokenType tokentype) {
    switch (tokentype) {
        case TokenType::BINARY_OP:
            return "BINARY_OP";
        case TokenType::UNARY_OP:
            return "UNARY_OP";
        case TokenType::IDENTIFIER:
            return "IDENTIFIER";
        case TokenType::STRING:
            return "STRING";
        case TokenType::IF:
            return "IF";
        case TokenType::ELSE:
            return "ELSE";
        case TokenType::WHILE:
            return "WHILE";
        case TokenType::RETURN:
            return "RETURN";
        case TokenType::FOR:
            return "FOR";
        case TokenType::LET:
            return "LET";
        case TokenType::VAR:
            return "VAR";
        case TokenType::CLASS:
            return "CLASS";
        case TokenType::L_BRACE:
            return "L_BRACE";
        case TokenType::R_BRACE:
            return "R_BRACE";
        case TokenType::INVALID:
            return "INVALID";
        case TokenType::NUMERIC:
            return "NUMERIC";
        case TokenType::FUNCTION:
            return "FUNCTION";
        case TokenType::L_PARENTHESES:
            return "L_PARENTHESES";
        case TokenType::R_PARENTHESES:
            return "R_PARENTHESES";
        case TokenType::END_OF_STATEMENT:
            return "END_OF_STATEMENT";
        case TokenType::END_OF_STREAM:
            return "END_OF_STREAM";
        case TokenType::COMMA:
            return "COMMA";
        case TokenType::SEMICOLON:
            return "SEMICOLON";
        case TokenType::DOT:
            return "DOT";
        default:
            //VERIFY_NOT_REACHED();
            return "INVALID";
        break;
    }
}

void Lexer::IdentifyToken(std::string& lexemetoid, Token& tokentoid) {

    tokentoid.Lexeme = lexemetoid;
    if (isNumeric(lexemetoid)) {
            tokentoid.Type = TokenType::NUMERIC;
            return;
        }
    auto LexemeType = TokenTypeMap.find(lexemetoid);

            if (LexemeType != TokenTypeMap.end()) {
                auto lexemetype = LexemeType->second;
                tokentoid.Type = lexemetype;
            } else {
                tokentoid.Type = TokenType::IDENTIFIER;
            }
            }

