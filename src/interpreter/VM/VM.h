#pragma once
#include "ExecutionState.h"
#include "../Bytecode/BytecodeStream.h"

namespace js {
    namespace Interpreter {
        class VM {
            using Bytecode = uint8_t;
        public:
            explicit VM(BytecodeStream* bytecodeStream): m_stream(bytecodeStream) {}
            ~VM() = default;
            void Run();

            void initialize();
            


             
            
        private:
            BytecodeStream* m_stream;
            ExecutionState* m_execState {new ExecutionState()};
        };

    };

}
