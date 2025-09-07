#pragma once

#include "BasicBlock.h"
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

        private:
            BytecodeGenerator() = default;
        public:
            static BytecodeGenerator* Create() {
                return new BytecodeGenerator();
            }

            ~BytecodeGenerator() {
                delete m_stream;
            }

            BytecodeStream* ExtractBytecodeStream() const {
                return m_stream;
            }
            template <typename... Args> void BuildCommand(Opcode op, Args... args) {
                *m_stream << static_cast<uint8_t>(op);
                std::initializer_list<int>{(*m_stream << static_cast<uint8_t>(args), 0)...};
            }

            //TODO: Build Basic Blocks from ASTNode Objects
            void build_basic_blocks();

            //TODO: Convert BasicBlock relative jumps to absolute jumps
            void convert_relative_jumps_to_absolute();

            //TODO: Convert BasicBlock relative addresses to absolute addresses
            void convert_relative_addresses_to_absolute();



        private:
            void emitBytecode (int opCode);
            void emitSingleCommand();




        private:
            BytecodeStream* m_stream {BytecodeStream::Create()};
            std::vector<BasicBlock> m_blocks {};

        }; // class BytecodeGenerator
    };  // namespace Interpreter
}; // namespace js

