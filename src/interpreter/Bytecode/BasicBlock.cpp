#include "BasicBlock.h"

namespace js {
    namespace Interpreter {
        template <typename... Ts>
        void BasicBlock<Ts...>::parse_instructions() {
                for (auto& inst : m_insn) {
                    for (const auto& arg : inst.args()) {
                        std::visit([=](auto&& value) {
                            using T = std::decay_t<decltype(value)>;
                            if constexpr (std::is_same_v<T, uint8_t>) {
                                m_bytecodes.push_back(static_cast<Bytecode>(value));
                            } else if constexpr (std::is_same_v<T, uint16_t>) {
                                m_bytecodes.push_back(static_cast<Bytecode>(value & 0xFF));
                                m_bytecodes.push_back(static_cast<Bytecode>((value >> 8) & 0xFF));
                            } else if constexpr (std::is_same_v<T, uint32_t>) {
                                m_bytecodes.push_back(static_cast<Bytecode>(value & 0xFF));
                                m_bytecodes.push_back(static_cast<Bytecode>((value >> 8) & 0xFF));
                                m_bytecodes.push_back(static_cast<Bytecode>((value >> 16) & 0xFF));
                                m_bytecodes.push_back(static_cast<Bytecode>((value >> 24) & 0xFF));
                            } else if constexpr (std::is_same_v<T, uint64_t>) {
                                m_bytecodes.push_back(static_cast<Bytecode>(value & 0xFF));
                                m_bytecodes.push_back(static_cast<Bytecode>((value >> 8) & 0xFF));
                                m_bytecodes.push_back(static_cast<Bytecode>((value >> 16) & 0xFF));
                                m_bytecodes.push_back(static_cast<Bytecode>((value >> 24) & 0xFF));
                                m_bytecodes.push_back(static_cast<Bytecode>((value >> 32) & 0xFF));
                                m_bytecodes.push_back(static_cast<Bytecode>((value >> 40) & 0xFF));
                                m_bytecodes.push_back(static_cast<Bytecode>((value >> 48) & 0xFF));
                                m_bytecodes.push_back(static_cast<Bytecode>((value >> 56) & 0xFF));
                            }
                        }, arg);
                    }
                }
            }//parse_instructions



    } // namespace Interpreter
} // namespace js