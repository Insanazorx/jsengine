#pragma once
#include "Flag.h"
#include "Register.h"
#include "../bytecode/BytecodeProgram.h"
#include "../RuntimeObjects/ExecutionContext.h"

namespace js::Interpreter {
    class ExecutionContext;

    struct ExecutionState : VMObject{
        explicit ExecutionState(VM& vm_ref) : VMObject(vm_ref) {}
        ~ExecutionState() override = default;

        bool is_execution_state() const override { return true; }

        Accumulator acc {Accumulator(vm)};
        ProgramCounter pc {};
        LinkRegister lr {};
        FlagSet flags {FlagSet(vm)};
        ExecutionContext* current_context {new ExecutionContext()};

        void save_state_and_push_to_stack () {

        }
    };
}
