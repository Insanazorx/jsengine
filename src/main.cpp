#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>
#include <string>

#include "./ast/ASTNodeFactory.h"
#include "interpreter/VM/VM.h"


int main () {

    std::string file_path = "/Users/insanazor/Desktop/esprima/ast.json";
    std::ifstream ast_file(file_path);
    if (!ast_file.is_open()) {
        std::cerr << "Error opening file: " << file_path << std::endl;
        return 1;
    }
    //PORNO
    std::cout << "porno";
    nlohmann::json json_data;
    try {
        ast_file >> json_data;
    } catch (const std::exception& e) {
        std::cerr << "JSON parse hatası: " << e.what() << std::endl;
        return 1;
    }
    //std::cout << json_data.dump(4);

    js::ASTNode* tl_node = js::AstNodeFactory::CreateFromJson(json_data);
    std::cout << tl_node->toJson().dump(4);



    auto generator = js::Interpreter::BytecodeGenerator::Create();

    auto node = js::BinaryOpNode::Create();

    node->GenerateBytecode(*generator);

    auto result = generator->ExtractBytecodeStream();

    (*result).print_bytecodes();


    auto vm = new js::Interpreter::VM(result);













    /*std::string expr("if (d-(a+b)*c) { return 1 + 2 + 3 ; }");
    Lexer lexer(expr);
    lexer.Scan();
    for (auto token : lexer.Tokens())
    DEBUG("Type of " << token.Lexeme << ": " << Lexer::StringifyTokenType(token));

    auto& tokens = lexer.Tokens();
    auto ctx = ParserContext::Create(Parser::Instance());
    Parser::Initialize(tokens, ctx);
    Parser::Instance()->ParseInGlobalContextFromTokens();
    */

    return 0;

};