#pragma once
#include <vector>
#include <iostream>

#include "Bytecodes.h"

namespace js {
    namespace Interpreter {



        template <typename... Ts>
        class Instruction {
            using Operand = std::variant<uint8_t, uint16_t, uint32_t, uint64_t>;
        public:
            explicit Instruction(Ts... args):
               m_args({Operand(std::forward<Ts>(args))...}) {}

            virtual ~Instruction() = default;
            virtual Opcode opcode() const {return Opcode::NOP;}
            virtual int len() const {return -1;}


            void print() const {
                std::cout << "Opcode: " << static_cast<int>(opcode()) << " Args: ";
                for (const auto& arg : m_args) {
                    std::visit([](auto&& value) { std::cout << +value << " "; }, arg);
                }
                std::cout << "\n";
            }
            const std::vector<Operand>& args() const { return m_args; }

        protected:
            std::vector<Operand> m_args;
            uint32_t m_address {0};
        };


#define INSTRUCTION_CLASS(name, code, length, ...)                                  \
    class name##_INST : public Instruction<__VA_ARGS__> {                           \
    private:                                                                        \
        template <typename... Args>                                                 \
        name##_INST(Args... args) : Instruction(args...) {}                         \
    public:                                                                         \
        template <typename... Args>                                                 \
        static name##_INST* Create(Args... args) {                                  \
            return new name##_INST(args...);                                        \
        }                                                                           \
        ~name##_INST() override = default;                                          \
        size_t fixed_length {length};                                               \
        Opcode opcode_value {Opcode::name};                                         \
    };

        X_FOR_BYTECODES_WITH_TYPED_ARGS(INSTRUCTION_CLASS)
    }
}
