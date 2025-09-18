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

#define p64(x) static_cast<uint64_t>(x)
#define p32(x) static_cast<uint32_t>(x)
#define p16(x) static_cast<uint16_t>(x)
#define p8(x) static_cast<uint8_t>(x)

    using namespace js::Interpreter;


#define BUILD_COMMAND(bb, x) bb->add_instruction(x);



    // let x = 10;
    // while
    // if (x > 5) {
    //   x=x-2
    // }
    // x *= 2

    auto bb = BasicBlock::Create(Label::Create("start"));
    BUILD_COMMAND(bb, LOAD_ACCUMULATOR_IMM8_INST::Create(p8(5)));
    BUILD_COMMAND(bb, STORE_ACCUMULATOR_INST::Create(p8(1)));
    BUILD_COMMAND(bb, XOR_INST::Create(p8(0)));
    BUILD_COMMAND(bb, LOAD_ACCUMULATOR_IMM8_INST::Create(p8(10)));
    BUILD_COMMAND(bb, STORE_ACCUMULATOR_INST::Create(p8(2))); //x
    BUILD_COMMAND(bb, COMPARE_INST::Create(p8(1)));

    auto bb_true= BasicBlock::Create(Label::Create("if_true"));
    BUILD_COMMAND(bb_true, XOR_INST::Create(p8(0)));
    BUILD_COMMAND(bb_true, LOAD_ACCUMULATOR_IMM8_INST::Create(p8(2)));
    BUILD_COMMAND(bb_true, STORE_ACCUMULATOR_INST::Create(p8(3)));
    BUILD_COMMAND(bb_true, LOAD_ACCUMULATOR_REG_INST::Create(p8(2)));
    BUILD_COMMAND(bb_true, SUBTRACT_INST::Create(p8(3)));
    BUILD_COMMAND(bb_true, STORE_ACCUMULATOR_INST::Create(p8(2)));

    auto bb_false = BasicBlock::Create(Label::Create("if_false"));
    BUILD_COMMAND(bb_false, XOR_INST::Create(p8(0)));
    BUILD_COMMAND(bb_false, LOAD_ACCUMULATOR_REG_INST::Create(p8(2)));
    BUILD_COMMAND(bb_false, MULTIPLY_INST::Create(p8(1)));
    BUILD_COMMAND(bb_false, STORE_ACCUMULATOR_INST::Create(p8(2)));
    BUILD_COMMAND(bb_false, DEBUG_PRINT_INST::Create(p8(2)));
    BUILD_COMMAND(bb_false, HALT_INST::Create());

    BUILD_COMMAND(bb, LOAD_ACCUMULATOR_IMM64_INST::Create(p64(bb->calculate_size()+bb_true->calculate_size()+ 11)));
    BUILD_COMMAND(bb, JUMP_IF_LTE_INST::Create(p8(0)));

    bb->label()->set_start_address(0);
    bb_true->label()->set_start_address(bb->label()->start_address() + bb->calculate_size());
    bb_false->label()->set_start_address(bb->label()->start_address() + bb->calculate_size() + bb_true->calculate_size());

    bb->parse_instructions();
    bb_true->parse_instructions();
    bb_false->parse_instructions();

    bb->print_detailed();
    bb_true->print_detailed();
    bb_false->print_detailed();

    auto bytecode_program = BytecodeProgram::Create();
    *bytecode_program << *bb->bytecodes();
    *bytecode_program << *bb_true->bytecodes();
    *bytecode_program << *bb_false->bytecodes();

    auto vm = VM(bytecode_program);
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