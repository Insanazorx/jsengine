#pragma once
#include <vector>

#include "BytecodeProgram.h"
#include "Instruction.h"
#include "Label.h"

namespace js {
    namespace Interpreter {
        using Bytecode = uint8_t;

        class BasicBlock {
        private:
            explicit BasicBlock(Label* label) : m_label(label) {}
        public:
            static BasicBlock* Create(Label* label) {
                return new BasicBlock(label);
            }
            ~BasicBlock() = default;

            void make_true_branch(BasicBlock* true_branch) {m_true_branch = true_branch;}
            void make_false_branch(BasicBlock* false_branch) {m_false_branch = false_branch;}

            void add_instruction(Instruction* insn) {
                m_insn.push_back(insn);
            }

            Label* label() {return m_label;}


            BytecodeProgram* bytecodes() const {return m_bytecodes;}


            int calculate_size() const {
                int size = 0;
                for (auto insn : m_insn) {
                    size += insn->len();
                }
                return size;
            }

            void merge(BasicBlock* other) {
                for (auto insn : other->m_insn) {
                    m_insn.push_back(insn);
                }
                other->m_insn.clear();
                other->~BasicBlock();
            }

            void mangle_if_needed();

            // Instructions as mangle points (jump e.g)
            void add_insn_to_mangle(Instruction* insn) {m_insns_to_mangle.push_back(insn);}
            const std::vector<Instruction*>& insns_to_mangle() const {return m_insns_to_mangle;}


            void parse_instructions();

            void print_straight() const;
            void print_detailed() const;

        private:
            Label* m_label {};

            std::vector<Instruction*> m_insn {};
            BytecodeProgram* m_bytecodes {BytecodeProgram::Create()};

            std::optional<BasicBlock*> m_true_branch {nullptr};
            std::optional<BasicBlock*> m_false_branch {nullptr};
            std::optional<BasicBlock*> m_unconditional_branch {nullptr};

            std::vector<Instruction*> m_insns_to_mangle {};

        };//class BasicBlock
    }// namespace Interpreter
}// namespace js