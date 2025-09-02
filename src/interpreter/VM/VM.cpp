#pragma once
#include "VM.h"

#include "../../frontend/debug.h"
#include "../Bytecode/BytecodeGenerator.h"
#include "Register.h"

namespace js{
    namespace Interpreter {

            DECLARE_HANDLER(VM::LOAD_ACCUMULATOR_REG, Register& reg){}
            DECLARE_HANDLER(VM::LOAD_ACCUMULATOR_IMM, Value& imm){}
            DECLARE_HANDLER(VM::STORE_ACCUMULATOR, Register& reg){}
            DECLARE_HANDLER(VM::ADD, Register& reg){}
            DECLARE_HANDLER(VM::SUBTRACT, Register& reg){}
            DECLARE_HANDLER(VM::MULTIPLY, Register& reg){}
            DECLARE_HANDLER(VM::DIVIDE, Register& reg){}
            DECLARE_HANDLER(VM::JUMP, Address addr){}
            DECLARE_HANDLER(VM::COMPARE, Register& reg) {}
            DECLARE_HANDLER(VM::TEST, Register& reg){}
            DECLARE_HANDLER(VM::JUMP_IF_EQUAL, Address addr){}
            DECLARE_HANDLER(VM::JUMP_IF_ZERO, Address addr){}
            DECLARE_HANDLER(VM::JUMP_UNLESS_ZERO, Address addr){}
            DECLARE_HANDLER(VM::CALL_FUNCTION, Name& name){}
            DECLARE_HANDLER(VM::LOAD_NAMED_PROPERTY, Object_Descriptor od, Name& name){}
            DECLARE_HANDLER(VM::LOAD_KEYED_PROPERTY, Object_Descriptor od, Register& reg){}
            DECLARE_HANDLER(VM::LOAD_INDEXED_PROPERTY, Object_Descriptor od, Slot slot){}
            DECLARE_HANDLER(VM::CREATE_OBJECT, Name& name){}
            DECLARE_HANDLER(VM::CREATE_CONTEXT){}
            DECLARE_HANDLER(VM::NOP){}
            DECLARE_HANDLER(VM::RETURN){}
            DECLARE_HANDLER(VM::XOR, Register& reg){}
            DECLARE_HANDLER(VM::AND, Register& reg){}
            DECLARE_HANDLER(VM::OR, Register& reg){}
            DECLARE_HANDLER(VM::NEGATE, Register& reg){}
            DECLARE_HANDLER(VM::MODULO, Register& reg){}
            DECLARE_HANDLER(VM::SHIFT_LEFT, Value& imm){}
            DECLARE_HANDLER(VM::SHIFT_RIGHT, Value& imm){};
            DECLARE_HANDLER(VM::CREATE_FUNCTION, Name& name){};
            DECLARE_HANDLER(VM::CREATE_ARRAY, Name& name){};
            DECLARE_HANDLER(VM::SAVE_STATE){};
            DECLARE_HANDLER(VM::RESTORE_STATE){};
    }
}
