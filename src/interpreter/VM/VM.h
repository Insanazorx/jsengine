#pragma once
#include "ExecutionState.h"
#include "Stack.h"
#include "../Bytecode/BytecodeStream.h"
#include "../Bytecode/Bytecodes.h"

namespace js {
    namespace Interpreter {


        class VM {
            using Bytecode = uint8_t;
            using RegisterSet = std::array<Register*, 256>;
        public:
            explicit VM(BytecodeStream* bytecodeStream): m_stream(bytecodeStream) {}
            ~VM() = default;

            void run();

            void make_register_exist(uint8_t id) {
                if (id >= m_registers.size()) {
                    std::cerr << "Invalid register ID: " << static_cast<int>(id) << std::endl;
                    VERIFY_NOT_REACHED();
                }
                if (m_registers[id] == nullptr)
                    m_registers[id] = Register::CreateWithSpecificId(*this, id);
            }

            Register*& register_by_id (uint8_t id) {
                return m_registers.at(id);
            }


            ExecutionState* save_state () {

            }
            void push_stack();
            void pop_stack();

            void initialize();
            

            X_FOR_BYTECODES_WITH_TYPED_ARGS(HANDLER_PROTOTYPE)
             
            
        private:
            BytecodeStream* m_stream;
            RegisterSet m_registers {};
            ExecutionState* m_exec_state {new ExecutionState(*this)};
            Stack* m_stack {new Stack(*this)};
            bool m_running {false};
            //TODO: runtime heap for objects, functions, arrays, etc.
            //TODO: runtime objects

        };

    };

}
