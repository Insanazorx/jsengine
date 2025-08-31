#pragma once
#include "BytecodeGenerator.h"
#include  "BytecodeStream.h"

namespace js {
    namespace Interpreter {

        
        DECLARE_HANDLER(LOAD_ACCUMULATOR_REG, Register&){}
        DECLARE_HANDLER(LOAD_ACCUMULATOR_IMM, Value&){}
        DECLARE_HANDLER(STORE_ACCUMULATOR, Register&){}
        DECLARE_HANDLER(ADD, Register&){}
        DECLARE_HANDLER(SUBTRACT, Register&){}
        DECLARE_HANDLER(MULTIPLY, Register&){}
        DECLARE_HANDLER(DIVIDE, Register&){}
        DECLARE_HANDLER(JUMP, Address){}
        DECLARE_HANDLER(COMPARE, Register&){}
        DECLARE_HANDLER(TEST, Register&){}
        DECLARE_HANDLER(JUMP_IF_EQUAL, Address){}
        DECLARE_HANDLER(JUMP_IF_ZERO, Address){}
        DECLARE_HANDLER(JUMP_UNLESS_ZERO, Address){}
        DECLARE_HANDLER(CALL_FUNCTION, Name&){}
        DECLARE_HANDLER(LOAD_NAMED_PROPERTY, Object_Descriptor, Name&){}
        DECLARE_HANDLER(LOAD_KEYED_PROPERTY, Object_Descriptor, Register&){}
        DECLARE_HANDLER(LOAD_INDEXED_PROPERTY, Object_Descriptor, Slot){}
        DECLARE_HANDLER(CREATE_OBJECT){}
        DECLARE_HANDLER(CREATE_CONTEXT){}
        DECLARE_HANDLER(NOP){}
        DECLARE_HANDLER(RETURN){}
    }
}

