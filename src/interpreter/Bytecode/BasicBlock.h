#pragma once
#include <vector>

#include "Instruction.h"
#include "Label.h"

namespace js {
    namespace Interpreter {
        using Bytecode = uint8_t;
        #define VARIADIC uint8_t, uint16_t, uint32_t, uint64_t
        class BasicBlock {
        private:
            BasicBlock() = default;
        public:
            static BasicBlock* Create() {
                return new BasicBlock();
            }
            ~BasicBlock() = default;

            void make_next_block(BasicBlock* next) {m_next_block = next;}
            void make_true_branch(BasicBlock* true_branch) {m_true_branch = true_branch;}
            void make_false_branch(BasicBlock* false_branch) {m_false_branch = false_branch;}

            void add_instruction(const Instruction& insn) {m_insn.push_back(insn);}
            void set_start_address(int address) {m_start_address_absolute = address;}
            void set_label(Label* label) {m_label = label;}

            void relative_to_absolute_address_conversion(int base_address) {
                m_start_address_absolute += base_address;
            }

            void parse_instructions();
            void print() const;

        private:
            Label* m_label {};

            std::vector<Instruction> m_insn {};
            std::vector<Bytecode> m_bytecodes {};

            BasicBlock* m_true_branch {nullptr};
            BasicBlock* m_false_branch {nullptr};
            BasicBlock* m_next_block {nullptr};

            int m_start_address_absolute {0};


        };//class BasicBlock
    }// namespace Interpreter
}// namespace js