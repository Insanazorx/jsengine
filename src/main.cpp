#include "Lexer.h"
#include "Context.h"
#include "debug.h"
#include "Parser.h"

using namespace JSLib;

int main () {
    std::string expr("if (d-(a+b)*c) { return 1 + 2 + 3 ; }");
    Lexer lexer(expr);
    lexer.Scan();
    for (auto token : lexer.Tokens())
    DEBUG("Type of " << token.Lexeme << ": " << Lexer::StringifyTokenType(token));

    auto& tokens = lexer.Tokens();
    auto ctx = ParserContext::Create(Parser::Instance());
    Parser::Initialize(tokens, ctx);
    Parser::Instance()->ParseInGlobalContextFromTokens();


    return 0;

};