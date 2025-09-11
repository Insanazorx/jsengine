#pragma once
#include <vector>
#include <iostream>

#include "Bytecodes.h"

namespace js {
    namespace Interpreter {

#define p64(x) static_cast<uint64_t>(x)
#define p32(x) static_cast<uint32_t>(x)
#define p16(x) static_cast<uint16_t>(x)
#define p8(x) static_cast<uint8_t>(x)


        class Instruction {
        public:
            Instruction() = default;
            virtual ~Instruction() = default;
            virtual Opcode opcode() const {return Opcode::NOP;}
            virtual int len() const {return -1;}

            template <typename Ptr>
            static inline typename std::remove_pointer<Ptr>::type::TupleType& GET_OPERANDS(Ptr insn) {
                using T = typename std::remove_pointer<Ptr>::type;
                return static_cast<T*>(insn)->get_operands();
            }

            virtual void print() const {};
        protected:

            uint32_t m_address {0};
        };

#define INSTRUCTION_CLASS(name, code, length, ...)                                          \
    class name##_INSTRUCTION : public Instruction {                     \
    private:                                                                                \
        template <typename... Args>                                                         \
        name##_INSTRUCTION(Args... args) : m_operands (std::make_tuple(args...)) {}                   \
    public:                                                                                 \
        using TupleType = std::tuple<__VA_ARGS__>;                                \
        template <typename... Args>                                                             \
        static name##_INSTRUCTION* Create(Args... args) {                                       \
        return new name##_INSTRUCTION(args...);                                             \
    }                                                                                       \
        ~name##_INSTRUCTION() override = default;                                               \
        void print() const override{\
            std::cout << "Instruction: " << #name << " Operands: ";\
            std::apply([](auto&&... args) { ((std::cout << args << " "), ...); }, m_operands);\
            std::cout << std::endl;\
        }                          \
        Opcode opcode() const override {return Opcode::name;}                                   \
        int len() const override {return length;}                                               \
        auto& get_operands() {return m_operands;}                                   \
    private:                                                                                \
        TupleType m_operands;                                                           \
    };



        X_FOR_BYTECODES_WITH_TYPED_ARGS(INSTRUCTION_CLASS)




    }
}
