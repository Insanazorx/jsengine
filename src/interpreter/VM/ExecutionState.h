#pragma once
#include "Register.h"
#include "VM.h"
#include "../bytecode/BytecodeStream.h"
namespace js::Interpreter {

    struct ExecutionState {
        BytecodeStream* stream {};
        int pc = 0;
        std::vector<Register>& registers_copy;
        Register accumulator;



    };
}