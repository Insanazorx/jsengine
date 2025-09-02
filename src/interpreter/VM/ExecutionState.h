#pragma once
#include "Flag.h"
#include "Register.h"
#include "../bytecode/BytecodeStream.h"
namespace js::Interpreter {

    struct ExecutionState : VMObject{
        explicit ExecutionState(VM& vm_ref) : VMObject(vm_ref) {}
        ~ExecutionState() override = default;

        bool is_execution_state() const override { return true; }

        Accumulator acc {Accumulator(vm)};
        ProgramCounter pc {};
        LinkRegister lr {};
        FlagSet flags {FlagSet(vm)};

        void save_state_and_push_to_stack () {

        }
    };
}