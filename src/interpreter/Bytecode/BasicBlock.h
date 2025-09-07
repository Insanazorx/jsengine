#pragma once
#include <vector>

namespace js {
    namespace Interpreter {
        using Bytecode = uint8_t;

        class BasicBlock {
        private:
            BasicBlock() = default;
        public:
            static BasicBlock* Create() {
                return new BasicBlock();
            }
            ~BasicBlock() = default;

        private:
            std::vector<Bytecode> m_bytecodes {};

            int m_start_address {0};
            int last_relative_address {0};
        };//class BasicBlock
    }// namespace Interpreter
}// namespace js