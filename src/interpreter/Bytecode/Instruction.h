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


            virtual void print() const {}
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
            ~InstructionDeriver() override = default;
            void print() const override{}
        };


#define INSTRUCTION_CLASS(name, code, length, ...)                                              \
    class name##_INST : public InstructionDeriver<__VA_ARGS__> {                                \
    protected:                                                                                  \
        template <typename... Args>                                                             \
        name##_INST(Args... args) : InstructionDeriver(args...) {}                              \
    public:                                                                                     \
    template <typename... Args>                                                                 \
    static name##_INST* Create(Args... args) {                                                  \
        return new name##_INST(args...);                                                        \
    }                                                                                           \
    ~name##_INST() override = default;                                                          \
                                                                                                \
    Opcode opcode() const override {return Opcode::name;}                                       \
    int len() const override {return length;}                                                   \
    void print() const override {                                                               \
        std::cout << "Opcode: " << +opcode() << " Args: ";                                      \
        for (const auto& arg : m_args) {                                                        \
            std::visit([](auto&& value) { std::cout << std::hex << +value << " "; }, arg);      \
        }                                                                                       \
        std::cout << " | "<< #name << "\n";                                                     \
        }                                                                                       \
    };
        X_FOR_BYTECODES_WITH_TYPED_ARGS(INSTRUCTION_CLASS)

#define DEFERRED_INSTRUCTION_CLASS(name, code, length, ...)                                             \
    class DEFERRED_##name##_INST : public name##_INST {                                                 \
    protected:                                                                                          \
        template<typename... Args>                                                                      \
        DEFERRED_##name##_INST(Label* label, Args... args) : name##_INST(args...), m_label(label) {}    \
    public:                                                                                             \
        template <typename... Args>                                                                     \
        static DEFERRED_##name##_INST* Create(Label* label) {                                           \
            return new DEFERRED_##name##_INST();                                                        \
        }                                                                                               \
        ~DEFERRED_##name##_INST() override = default;                                                   \
        Instruction* manipulate_deferred_assignment() override {                                        \
                                                                                   \
            return nullptr;                                                                             \
        }                                                                                               \
    private:                                                                                            \
        Label* m_label {}                                                                               \
                                                                                                        \
};
    }
}
