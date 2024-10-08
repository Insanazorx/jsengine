#include "Lexer.h"
#include "debug.h"

using namespace JSLib;

void Lexer::Scan() {
    Std::String chars("+ - * / = ; || << ( ) { } >> < > != == <= >=");
    Std::Vector<Std::String> specials = chars.split(' ');
    Std::Vector<int> poses; 

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

void Lexer::IdentifyToken(Std::String& lexemetoid, Token& tokentoid) {
        if (lexemetoid.isNumeric())
            tokentoid.Type = TokenType::NUMERIC;
        else if (lexemetoid=='+' || lexemetoid == '-'|| lexemetoid == '/'|| lexemetoid == '*' || lexemetoid == ';'
        || lexemetoid == '%' || lexemetoid == '<' || lexemetoid == '>' || lexemetoid == "||" || lexemetoid == "&&" 
        || lexemetoid == '!' || lexemetoid == "!=" || lexemetoid == "==" || lexemetoid == "<=" || lexemetoid == ">="
        || lexemetoid == '&' || lexemetoid == '~' || lexemetoid == '^' )
            tokentoid.Type = TokenType::BINARY_OP;
        else if (lexemetoid == "return") {tokentoid.Type = TokenType::RETURN;} 
        else if (lexemetoid == "function") {tokentoid.Type = TokenType::FUNCTION;}
        else if (lexemetoid == "var") {tokentoid.Type = TokenType::VAR;}
        else if (lexemetoid == "let") {tokentoid.Type = TokenType::LET;}
        else if (lexemetoid == "if") {tokentoid.Type = TokenType::IF;} 
        else if (lexemetoid == "else") {tokentoid.Type = TokenType::ELSE;}
        else if (lexemetoid == "for") {tokentoid.Type = TokenType::FOR;} 
        else if (lexemetoid == "while") {tokentoid.Type = TokenType::WHILE;}
        else if (lexemetoid == "class") {tokentoid.Type = TokenType::CLASS;}
        else
            tokentoid.Type = TokenType::IDENTIFIER;
}

