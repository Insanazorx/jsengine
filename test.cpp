#include "Lexer.h"
#include "AST.h"
#include "debug.h"
#include "Parser.h"

using namespace JSLib;
using namespace Std;

int main (void) {
    String expr("if (a b c)");
    Lexer lexer;
    lexer.SetExpr(expr);
    lexer.Scan();
    for (auto token: lexer.Tokens()); 
        //DEBUG (token.Lexeme.c_str() << " ===> " << (token.Type == TokenType:: ? "INTEGER" : "OTHER"));
    

    Vector<Token*>& tokens = lexer.TokensForParser();
    Parser* parser = Parser::GetInstance(tokens);
    parser->GetTopLevelASTNode();
    parser->Analyze();

    return 0;

};