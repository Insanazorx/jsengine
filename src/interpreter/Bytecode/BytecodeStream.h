#pragma once
#include <iostream>
#include <vector>

namespace js {
    namespace Interpreter {
        class BytecodeStream {
            friend class BytecodeGenerator;
            using Bytecode = uint8_t;
        private:
            BytecodeStream() = default;
        public:
            static BytecodeStream* Create() {
                return new BytecodeStream();
            }
            ~BytecodeStream() = default;

            Bytecode at(size_t index) const {
                if (index < m_bytecodes.size()) {
                    return m_bytecodes[index];
                }
                throw std::out_of_range("Index out of range");
            }

            Bytecode operator[] (size_t index) const {
                return at(index);
            }

            BytecodeStream* operator>>(const std::vector<Bytecode>& bytecodes) {
                std::copy(bytecodes.begin(), bytecodes.end(), std::back_inserter(m_bytecodes));
                return this;
            }


            BytecodeStream* operator<<(const std::vector<Bytecode>& bytecodes) {
                m_bytecodes.insert(m_bytecodes.end(), bytecodes.begin(), bytecodes.end());
                return this;
            }

            BytecodeStream* operator<<(const BytecodeStream& other) {
                m_bytecodes.insert(m_bytecodes.end(), other.m_bytecodes.begin(), other.m_bytecodes.end());
                return this;
            }

            BytecodeStream* operator<<(Bytecode single_bytecode) {
                m_bytecodes.push_back(single_bytecode);
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



        private:
            std::vector<Bytecode> m_bytecodes;
        };

    } // namespace Interpreter
} // namespace js