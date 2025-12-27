#pragma once
#include "Flag.h"
#include "Register.h"
#include "../bytecode/BytecodeProgram.h"
#include "../RuntimeObjects/ExecutionContext.h"

namespace js::Interpreter {
    class ExecutionContext;

    struct ExecutionState{
        explicit ExecutionState(VM& vm_ref) {}
        ~ExecutionState() = default;

        bool is_execution_state() const { return true; }

        Accumulator acc {};
        ProgramCounter pc {};
        LinkRegister lr {};
        FlagSet flags {FlagSet()};
        ExecutionContext* current_context {new ExecutionContext()};

        void save_state_and_push_to_stack () {

        }
    };
}
