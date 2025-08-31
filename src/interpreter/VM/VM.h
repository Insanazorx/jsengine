#pragma once
#include "ExecutionState.h"
#include "../Bytecode/BytecodeStream.h"
#include "../Bytecode/Bytecodes.h"

namespace js {
    namespace Interpreter {


        class VM {
            using Bytecode = uint8_t;
        public:
            explicit VM(BytecodeStream* bytecodeStream): m_stream(bytecodeStream) {}
            ~VM() = default;
            void Run();

            void initialize();
            

            HANDLER_PROTOTYPE(LOAD_ACCUMULATOR_REG, Register&);
            HANDLER_PROTOTYPE(LOAD_ACCUMULATOR_IMM, Value&);
            HANDLER_PROTOTYPE(STORE_ACCUMULATOR, Register&);
            HANDLER_PROTOTYPE(ADD, Register&);
            HANDLER_PROTOTYPE(SUBTRACT, Register&);
            HANDLER_PROTOTYPE(MULTIPLY, Register&);
            HANDLER_PROTOTYPE(DIVIDE, Register&);
            HANDLER_PROTOTYPE(JUMP, Address);
            HANDLER_PROTOTYPE(COMPARE, Register&);
            HANDLER_PROTOTYPE(TEST, Register&);
            HANDLER_PROTOTYPE(JUMP_IF_EQUAL, Address);
            HANDLER_PROTOTYPE(JUMP_IF_ZERO, Address);
            HANDLER_PROTOTYPE(JUMP_UNLESS_ZERO, Address);
            HANDLER_PROTOTYPE(CALL_FUNCTION, Name&);
            HANDLER_PROTOTYPE(LOAD_NAMED_PROPERTY, Object_Descriptor, Name&);
            HANDLER_PROTOTYPE(LOAD_KEYED_PROPERTY, Object_Descriptor, Register&);
            HANDLER_PROTOTYPE(LOAD_INDEXED_PROPERTY, Object_Descriptor, Slot);
            HANDLER_PROTOTYPE(CREATE_OBJECT);
            HANDLER_PROTOTYPE(CREATE_CONTEXT);
            HANDLER_PROTOTYPE(NOP);
            HANDLER_PROTOTYPE(RETURN);
            HANDLER_PROTOTYPE(XOR, Register&);
            HANDLER_PROTOTYPE(AND, Register&);
            HANDLER_PROTOTYPE(OR, Register&);
            HANDLER_PROTOTYPE(NEGATE, Register&);
            HANDLER_PROTOTYPE(MODULO, Register&);
            HANDLER_PROTOTYPE(SHIFT_LEFT, Register&, Value&);
            HANDLER_PROTOTYPE(SHIFT_RIGHT, Register& , Value&);
             
            
        private:
            BytecodeStream* m_stream;
            ExecutionState* m_execState {new ExecutionState()};
        };

    };

}
