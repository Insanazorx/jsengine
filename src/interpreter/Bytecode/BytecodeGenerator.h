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
        private:
            enum class OpCode : uint64_t{
                BRANCH,
                LOAD_STORE,
                ARITHMETIC,
                LOAD_OBJECT_PROP,
                RETURN,
                NOP,
            };
            enum class OpCodeType : uint64_t {

                LOAD,
                STORE,

                ADD,
                SUBTRACT,
                MULTIPLY,
                DIVIDE,

                JUMP,
                JUMP_IF_TRUE,
                JUMP_IF_FALSE,
                JUMP_IF_ZERO,

                CALL_FUNCTION,

                LOAD_NAMED_PROPERTY,
                LOAD_KEYED_PROPERTY,
                LOAD_INDEXED_PROPERTY
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

        private:
            void emitBytecode ();
            void emitSingleCommand();

            template <typename... Args> void BuildCommand(Args... args);

        private:
            BytecodeStream* m_BytecodeStream {BytecodeStream::Create()};

        }; // class BytecodeGenerator
    };  // namespace Interpreter
}; // namespace js