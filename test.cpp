#include "Lexer.h"
#include "Context.h"
#include "debug.h"
#include "Parser.h"

using namespace JSLib;
using namespace Std;

int main (void) {
    std::string expr("if (a b c)");
    Lexer lexer;
    lexer.SetExpr(expr);
    lexer.Scan();
    for (auto token: lexer.Tokens()); 
        //DEBUG (token.Lexeme.c_str() << " ===> " << (token.Type == TokenType:: ? "INTEGER" : "OTHER"));

    vector<Token*>& tokens = lexer.TokensForParser();
    auto ctx = ParserContext::Create(Parser::Instance(), &Parser::RecognizeStatementOrRedirectNode);
    Parser::Initialize(tokens, ctx);
    Parser::Instance()->Analyze();


    return 0;

};