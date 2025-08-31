#pragma once

#include "Bytecodes.h"
#include "BytecodeStream.h"


namespace js {
    namespace Interpreter {
        enum class GenerateBytecodeResult {
            SUCCESS = 0,
            FAILURE = 1,
            UNSPECIFIED_NODE = 2
        };



        class BytecodeGenerator {
            using Slot = uint8_t;
            using Address = uint32_t;
            using Object_Descriptor = uint16_t;
            using Name = std::string;
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

