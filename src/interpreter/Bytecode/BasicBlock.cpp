#include "BasicBlock.h"

#include <iomanip>
#include <sstream>
#include <iostream>

namespace js {
    namespace Interpreter {

        void BasicBlock::parse_instructions() {
                for (auto& inst : m_insn) {
                    *m_bytecodes<<(inst->opcode());
                    for (const auto& arg : inst->args()) {
                        std::visit([=](auto&& value) {
                            using T = std::decay_t<decltype(value)>;
                            if constexpr (std::is_same_v<T, uint8_t>) {
                                *m_bytecodes<<(static_cast<Bytecode>(value));
                            } else if constexpr (std::is_same_v<T, uint16_t>) {
                                *m_bytecodes<<(static_cast<Bytecode>(value & 0xFF));
                                *m_bytecodes<<(static_cast<Bytecode>((value >> 8) & 0xFF));
                            } else if constexpr (std::is_same_v<T, uint32_t>) {
                                *m_bytecodes<<(static_cast<Bytecode>(value & 0xFF));
                                *m_bytecodes<<(static_cast<Bytecode>((value >> 8) & 0xFF));
                                *m_bytecodes<<(static_cast<Bytecode>((value >> 16) & 0xFF));
                                *m_bytecodes<<(static_cast<Bytecode>((value >> 24) & 0xFF));
                            } else if constexpr (std::is_same_v<T, uint64_t>) {
                                *m_bytecodes<<(static_cast<Bytecode>(value & 0xFF));
                                *m_bytecodes<<(static_cast<Bytecode>((value >> 8) & 0xFF));
                                *m_bytecodes<<(static_cast<Bytecode>((value >> 16) & 0xFF));
                                *m_bytecodes<<(static_cast<Bytecode>((value >> 24) & 0xFF));
                                *m_bytecodes<<(static_cast<Bytecode>((value >> 32) & 0xFF));
                                *m_bytecodes<<(static_cast<Bytecode>((value >> 40) & 0xFF));
                                *m_bytecodes<<(static_cast<Bytecode>((value >> 48) & 0xFF));
                                *m_bytecodes<<(static_cast<Bytecode>((value >> 56) & 0xFF));
                            }
                        }, arg);
                    }
                }
            }//parse_instructions



        void BasicBlock::print_straight() const {

                if (m_bytecodes->empty()) {
                    std::cout << "[BasicBlock] bytecodes: (empty)\n";
                    return;
                }

                auto old_flags = std::cout.flags();
                auto old_fill  = std::cout.fill();

                constexpr std::size_t kPerLine = 16;
                std::cout << std::uppercase << std::hex << std::setfill('0');

                for (std::size_t i = 0; i < m_bytecodes->size(); ++i) {
                    if (i % kPerLine == 0) {
                        if (i != 0) std::cout << '\n';
                        std::cout << std::setw(6) << i << "  ";
                    }
                    unsigned byte = static_cast<unsigned>(m_bytecodes->at(i)) & 0xFFu;
                    std::cout << std::setw(2) << byte << ' ';
                }
                std::cout << '\n';

                std::cout.flags(old_flags);
                std::cout.fill(old_fill);

        }

        void BasicBlock::print_detailed() const {

            if (m_bytecodes->empty()) {
                std::cout << "[BasicBlock \"" << m_label->name() << "\"]"<< " (empty) \n ";
                return;
            }

            std::cout << "[BasicBlock \"" << m_label->name() << "\"] \n";

            std::vector<std::vector<Bytecode>> whole_printing_buffer;
            std::vector<Bytecode> printing_buffer;
            int arg_length = -2;
            for (int i = 0; i < m_bytecodes->size();) {
                arg_length = get_bytecode_length(static_cast<Opcode>(m_bytecodes->at(i)));

                if (arg_length == -1)
                    continue;
                if (arg_length == -2)
                    VERIFY_NOT_REACHED();

                printing_buffer.clear();
                for (size_t j = 0; j < arg_length; j++) {
                    if (i + j < m_bytecodes->size()) {
                        printing_buffer.push_back(m_bytecodes->at(i + j));
                    } else {
                        VERIFY_NOT_REACHED();
                    }
                }
                whole_printing_buffer.push_back(printing_buffer);
                i += arg_length;
            }
            int address = m_label->start_address();
            for (auto line : whole_printing_buffer) {
                auto it = opcode_to_string_list.find(static_cast<uint8_t>(line[0]));

                if (it == opcode_to_string_list.end())
                    VERIFY_NOT_REACHED();



                std::cout << std::setw(6) << std::hex << address << ": ";
                for (auto byte : line) {
                    std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte) << " ";
                }
                std::cout << " | " << it->second;
                std::cout << std::endl;
                address += line.size();
            }

        }//print_detailed

        void BasicBlock::mangle_if_needed() {

        }







    } // namespace Interpreter
} // namespace js