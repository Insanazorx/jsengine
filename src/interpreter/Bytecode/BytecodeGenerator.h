#pragma once

#include "BytecodeStream.h"


namespace js {
    namespace Interpreter {
        enum class GenerateBytecodeResult {
            SUCCESS = 0,
            FAILURE = 1,
            UNSPECIFIED_NODE = 2
        };



        class BytecodeGenerator {
        public:


            enum class OpCode : uint8_t {

            };
        private:
            BytecodeGenerator() = default;
        public:
            static BytecodeGenerator* Create() {
                return new BytecodeGenerator();
            }

            ~BytecodeGenerator() {
                delete m_BytecodeStream;
            }

            BytecodeStream* ExtractBytecodeStream() const {
                return m_BytecodeStream;
            }
            template <typename... Args> void BuildCommand(OpCode op, Args... args) {
                BytecodeStream* stream = BytecodeStream::Create();
                *stream << static_cast<BytecodeStream::Bytecode>(op);
                std::initializer_list<int>{(*stream << static_cast<BytecodeStream::Bytecode>(args), 0)...};
            }

        private:
            void emitBytecode (int opCode);
            void emitSingleCommand();




        private:
            BytecodeStream* m_BytecodeStream {BytecodeStream::Create()};

        }; // class BytecodeGenerator
    };  // namespace Interpreter
}; // namespace js

