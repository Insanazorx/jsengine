#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>
#include <string>

#include "Util.h"
#include "./ast/ASTNodeFactory.h"
#include "interpreter/Bytecode/Instruction.h"
#include "interpreter/VM/VM.h"


int main () {

    /*std::string file_path = "/Users/insanazor/Desktop/esprima/ast.json";
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

    js::AST::ASTNode* tl_node = js::AST::AstNodeFactory::CreateFromJson(json_data);
    std::cout << tl_node->toJson().dump(4);
*/


    auto generator = js::Interpreter::BytecodeGenerator::Create();

#define BUILD_COMMAND(op, ...) generator->BuildCommand(js::Interpreter::Opcode::op __VA_OPT__(,) __VA_ARGS__)
#define p64(x) static_cast<uint64_t>(x)
#define p32(x) static_cast<uint32_t>(x)
#define p16(x) static_cast<uint16_t>(x)
#define p8(x) static_cast<uint8_t>(x)

    BUILD_COMMAND(LOAD_ACCUMULATOR_IMM64, p64(0xdeadbeef));
    BUILD_COMMAND(STORE_ACCUMULATOR, p8(1));
    BUILD_COMMAND(LOAD_ACCUMULATOR_IMM32, p32(0xcafebabe));
    BUILD_COMMAND(ADD, p8(1));
    BUILD_COMMAND(STORE_ACCUMULATOR, p8(2));
    BUILD_COMMAND(DEBUG_PRINT, p8(2));
    BUILD_COMMAND(HALT);

    auto mul_inst = js::Interpreter::MULTIPLY_INST::Create(p8(13));
    auto bb = js::Interpreter::BasicBlock::Create();
    bb->add_instruction(*mul_inst);

    mul_inst->print();

    DEBUG("porno");

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