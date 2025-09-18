#pragma once
#include <iomanip>
#include <iostream>
#include <vector>

#include "Bytecodes.h"
#include "../../frontend/debug.h"


namespace js {
    namespace Interpreter {

        using Bytecode = uint8_t;

        class BytecodeProgram {

        private:
            BytecodeProgram() = default;
        public:
            static BytecodeProgram* Create() {
                return new BytecodeProgram();
            }
            ~BytecodeProgram() = default;

            Bytecode at(size_t index) const {
                if (index < m_bytecodes.size()) {
                    return m_bytecodes[index];
                }
                throw std::out_of_range("Index out of range");
            }

            BytecodeProgram& operator<<(const BytecodeProgram& other) {
                m_bytecodes.insert(m_bytecodes.end(), other.m_bytecodes.begin(), other.m_bytecodes.end());
                return *this;
            }

            BytecodeProgram& operator<<(Bytecode bytecode) {
                m_bytecodes.push_back(bytecode);
                return *this;
            }


            void print() {
                std::vector<std::vector<Bytecode>> whole_printing_buffer;
                std::vector<Bytecode> printing_buffer;
                int arg_length = -2;
                for (int i = 0; i < m_bytecodes.size();) {
                    arg_length = get_bytecode_length(static_cast<Opcode>(m_bytecodes[i]));

                    if (arg_length == -1)
                        continue;
                    if (arg_length == -2)
                        VERIFY_NOT_REACHED();

                    printing_buffer.clear();
                    for (size_t j = 0; j < arg_length; j++) {
                        if (i + j < m_bytecodes.size()) {
                            printing_buffer.push_back(m_bytecodes[i + j]);
                        } else {
                            VERIFY_NOT_REACHED();
                        }
                    }
                    whole_printing_buffer.push_back(printing_buffer);
                    i += arg_length;
                }
                int address = 0;
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

            }





            Bytecode operator[] (size_t index) const {
                return at(index);
            }

            int bytecode_count () const {
                return m_bytecodes.size();
            }

            void Clear() {
                m_bytecodes.clear();
            }

            auto begin() {
                return m_bytecodes.begin();
            }
            auto end() {
                return m_bytecodes.end();
            }

            bool empty() const {
                return m_bytecodes.empty();
            }

            int size () const {
                return m_bytecodes.size();
            }


        private:
            std::vector<Bytecode> m_bytecodes;
        };

    } // namespace Interpreter
} // namespace js