#pragma once

#include "BasicBlock.h"
#include "Bytecodes.h"
#include "BytecodeProgram.h"


namespace js {
    namespace Interpreter {
        class VM;

        enum class GenerateBytecodeResult {
            SUCCESS = 0,
            FAILURE = 1,
            UNSPECIFIED_NODE = 2
        };


        class BytecodeGenerator {
            using Slot = uint8_t;
            using Address = uint32_t;
            using Name = std::string;
        private:
            BytecodeGenerator() = default;
        public:
            static BytecodeGenerator* Create() {
                return new BytecodeGenerator();
            }

            ~BytecodeGenerator() {
                delete m_program;
            }

            BytecodeProgram* ExtractBytecodeStream() const {
                return m_program;
            }

            void add_basic_block(BasicBlock* block) {

                BasicBlock* prev = nullptr;
                if (!m_blocks.empty()) {
                    prev = m_blocks.back();
                }
                block->label()->set_start_address(prev ? prev->label()->start_address() + prev->bytecodes()->size() : 0);
                m_blocks.push_back(block);


            }

            void update_jump_instructions();

            Reg allocate_register(VM& vm);

            void free_register(VM& vm, Reg reg);

            void link_basic_blocks(BasicBlock* entry_block);

            void convert_relative_jumps_to_absolute();


        private:
            BytecodeProgram* m_program {BytecodeProgram::Create()};
            std::vector<BasicBlock*> m_blocks;

            std::unordered_map<std::string, uint32_t> m_label_to_address_map;

        }; // class BytecodeGenerator
    };  // namespace Interpreter
}; // namespace js

