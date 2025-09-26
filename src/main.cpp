#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>
#include <string>

#include "Util.h"
#include "./ast/ASTNodeFactory.h"
#include "interpreter/Bytecode/Instruction.h"
#include "interpreter/VM/VM.h"
#include <sys/mman.h>


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



    /*auto bb = BasicBlock::Create(Label::Create("start"));


    bb->label()->set_start_address(0);

    bb->parse_instructions();

    bb->print_detailed();


    auto bytecode_program = BytecodeProgram::Create();
    *bytecode_program << *bb->bytecodes();

    auto vm = VM(bytecode_program);
    vm.run(); */






    auto obj = JSObject::Create("MyObject");
    obj->SetProperty(1, std::make_shared<JSValue>(42));
    obj->SetProperty(2, std::make_shared<JSValue>("Hello, World!"));
    auto a = JSValue(obj);
    std::cout << std::hex << a.asObject() << std::endl;















    /*std::string expr("if (d-(a+b)*c) { return 1 + 2 + 3 ; }");*/

    return 0;

};