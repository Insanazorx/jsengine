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

            enum class OpCode : uint64_t{
                BRANCH = 0x1,
                LOAD_STORE = 0x2,
                ARITHMETIC = 0x3,
                LOAD_OBJECT_PROP = 0x4,
                RETURN = 0x5,
                NOP = 0x6,
            };
            enum class OpCodeType : uint64_t {

                LOAD = 0x1,
                STORE = 0x2,
                MOV = 0x3,

                ADD = 0x4,
                SUBTRACT = 0x5,
                MULTIPLY = 0x6,
                DIVIDE = 0x7,

                JUMP = 0x8,
                JUMP_IF_TRUE = 0x9,
                JUMP_IF_FALSE = 0x10,
                JUMP_IF_ZERO = 0x11,

                CALL_FUNCTION = 0x12,

                LOAD_NAMED_PROPERTY = 0x13,
                LOAD_KEYED_PROPERTY = 0x14,
                LOAD_INDEXED_PROPERTY = 0x15
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
            template <typename... Args> void BuildCommand(OpCode op, Args... args);

        private:
            void emitBytecode (int opCode);
            void emitSingleCommand();




        private:
            BytecodeStream* m_BytecodeStream {BytecodeStream::Create()};

        }; // class BytecodeGenerator
    };  // namespace Interpreter
}; // namespace js

