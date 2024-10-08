#pragma once
#include "StdLib.h"
#include "LexerTypes.h"

namespace JSLib {

class Lexer {
public:
    explicit Lexer() {};
    explicit Lexer(Std::String data) : m_Data(data){} ;

    void SetExpr(Std::String data) {m_Data = data;};

    void Scan();
    void IdentifyToken(Std::String& lexemetoid, Token& tokentoid);
    Std::Vector<Token>& Tokens() {return m_Tokens;}
    Std::Vector<Token*>& TokensForParser() {
        for (auto token : m_Tokens){
            Token* TokenPtr = &token;
            m_TokensForParser.push_back(TokenPtr);
        }
        return m_TokensForParser;
    } 

private:
    Std::String m_Data;
    Std::Vector<Token> m_Tokens;
    Std::Vector<Token*> m_TokensForParser;
};

}