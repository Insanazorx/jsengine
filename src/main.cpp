#include <iostream>
#include <fstream>
#include "Interpreter/Bytecode/BytecodeGenerator.h"
#include "Interpreter/GarbageCollector/Heap.h"

#define p64(x) static_cast<uint64_t>(x)
#define p32(x) static_cast<uint32_t>(x)
#define p16(x) static_cast<uint16_t>(x)
#define p8(x) static_cast<uint8_t>(x)

    using namespace js::Interpreter;


#define BUILD_COMMAND(bb, x) bb->add_instruction(x);

int main() {
    auto heap = new js::GarbageCollector::Heap();
    auto obj = heap->allocate<int>(42);
    do {  int a = 3+4; } while (false);
}

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
