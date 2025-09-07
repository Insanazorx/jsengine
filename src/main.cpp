#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>
#include <string>

#include "./ast/ASTNodeFactory.h"
#include "interpreter/RuntimeObjects/Value.h"
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

#define BUILD_COMMAND(op, ...) generator->BuildCommand(js::Interpreter::Opcode::op __VA_OPT__(,) __VA_ARGS__)

    BUILD_COMMAND(LOAD_ACCUMULATOR_IMM8, 10);
    BUILD_COMMAND(STORE_ACCUMULATOR, 1);
    BUILD_COMMAND(LOAD_ACCUMULATOR_IMM8, 20);
    BUILD_COMMAND(ADD, 1);
    BUILD_COMMAND(STORE_ACCUMULATOR, 2);
    BUILD_COMMAND(DEBUG_PRINT, 2);
    BUILD_COMMAND(HALT);


    auto bytecode_stream = generator->ExtractBytecodeStream();

    bytecode_stream->print();

    auto vm = js::Interpreter::VM(bytecode_stream);
    vm.run();

















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