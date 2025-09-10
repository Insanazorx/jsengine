#pragma once
#include <vector>
#include <iostream>

#include "Bytecodes.h"

namespace js {
    namespace Interpreter {



        class Instruction {
        public:
            using Operand = std::variant<uint8_t, uint16_t, uint32_t, uint64_t>;

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
            virtual const std::vector<Operand>& args() const { return m_args; }

        protected:
            std::vector<Operand> m_args;
            uint32_t m_address {0};
        };

        template <typename... Ts>
        class InstructionDeriver: public Instruction {
        public:
            explicit InstructionDeriver(Ts... args) {
                (m_args.emplace_back(std::forward<Ts>(args)), ...);
            }
        };


#define INSTRUCTION_CLASS(name, code, length, ...)                                  \
    class name##_INST : public InstructionDeriver<__VA_ARGS__> {                           \
    private:                                                                        \
        template <typename... Args>                                                     \
        name##_INST(Args... args) : InstructionDeriver(args...) {}                             \
    public:                                                                         \
    template <typename... Args>                                                     \
    static name##_INST* Create(Args... args) {                                      \
        return new name##_INST(args...);                                                \
    }                                                                               \
    ~name##_INST() override = default;                                              \
                                                                                \
    Opcode opcode() const override {return Opcode::name;}                       \
    int len() const override {return length;}                                   \
                                                                        \
};
        X_FOR_BYTECODES_WITH_TYPED_ARGS(INSTRUCTION_CLASS)

    }
}
