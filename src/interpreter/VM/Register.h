#pragma once
#include <cstdint>
#include <cstdlib>
#include <ctime>

#include "VMObject.h"

namespace js {
    namespace Interpreter {
        class Register : public VMObject {
        protected:
            //for allocating specific register
            explicit Register(VM& vm,uint8_t reg_id) : VMObject(vm), m_id(reg_id), in_use(true) {
            }

            //for allocating random register
            Register(VM& vm) : VMObject(vm), in_use(true) {
                srand(time(NULL));
                m_id = rand() % 256;
            }
        public:
            Register(const Register&) = default;
            Register& operator=(const Register&) = default;
            Register(Register&&) = delete;

            virtual ~Register() = default;

            static Register* CreateWithRandomId(VM& vm) {
                return new Register(vm);
            }
            static Register* CreateWithSpecificId(VM& vm, uint8_t reg_id) {
                return new Register(vm, reg_id);
            }
            void SetValue(uint64_t val) {m_value = val;}
            uint64_t GetValue() const {return m_value;}
            
            virtual uint8_t Id() const {return m_id;}
            virtual bool IsInUse() const {return in_use;}
            void SetInUse(bool use) {in_use = use;}
        protected:
            uint64_t m_value {0};
            uint8_t m_id {0};
            bool in_use {false};


        };
        class ProgramCounter final : public Register {
        public:
            ProgramCounter() : Register(vm, 0xFE) {}; // PC does not need a VM reference
            ~ProgramCounter() override = default;

            bool IsInUse() const override {return true;}
            void SetAddress(uint32_t addr) {m_address = addr;}
            uint32_t GetAddress() const {return m_address;}
            void Advance() {m_address++;}

        private:
            uint32_t m_address {0};
        };
        class Accumulator final : public Register {
        public:
            Accumulator(VM& vm) : Register(vm, 0) {};
            ~Accumulator() override = default;
            bool IsInUse() const override {return true;}
            void Reset() {m_value = 0;}
            void add_to_value(Register*& reg) {m_value += reg->GetValue();}
            void subtract_from_value(Register*& reg) {m_value -= reg->GetValue();}
            void multiply_with_value(Register*& reg) {m_value *= reg->GetValue();}
            void divide_with_value(Register*& reg) {m_value /= reg->GetValue();}
            void xor_with_value(Register*& reg) {m_value ^= reg->GetValue();}
            void and_with_value(Register*& reg) {m_value &= reg->GetValue();}
            void or_with_value(Register*& reg) {m_value |= reg->GetValue();}
            void negate_value() {m_value = -m_value;}
            void modulo_with_value(Register*& reg) {m_value %= reg->GetValue();}
            void shift_left_with_value(uint8_t imm) {m_value <<= imm;}
            void shift_right_with_value(uint8_t imm) {m_value >>= imm;}


        };

        class LinkRegister final : public Register {
        public:
            LinkRegister() : Register(vm, 0xFF) {};
            ~LinkRegister() override = default;
            bool IsInUse() const override {return true;}

        };

    }
}
