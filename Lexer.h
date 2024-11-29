#pragma once
#include "StdLib.h"
#include "LexerTypes.h"

namespace JSLib {

class Lexer {
public:
    Lexer()=default;
     Lexer(Std::std::string data) : m_Data(data) {} ;

    void SetExpr(Std::std::string data) {m_Data = data;};

    void Scan();
    void IdentifyToken(Std::std::string& lexemetoid, Token& tokentoid);
    Std::vector<Token>& Tokens() {return m_Tokens;}
    Std::vector<Token*>& TokensForParser() {
        for (auto token : m_Tokens){
            Token* TokenPtr = &token;
            m_TokensForParser.push_back(TokenPtr);
        }
        return m_TokensForParser;
    } 

private:
    Std::std::string m_Data;
    Std::vector<Token> m_Tokens;
    Std::vector<Token*> m_TokensForParser;
};

}