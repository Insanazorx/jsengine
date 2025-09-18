#pragma once
#include <cstdint>
#include <cstdlib>

#include "../../frontend/debug.h"
#include "VMObject.h"
#include "../RuntimeObjects/Value.h"

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
            void set8(uint64_t val) {m_value = val & 0xFF;}
            void set16(uint64_t val) {m_value = val & 0xFFFF;}
            void set32(uint64_t val) {m_value = val & 0xFFFFFFFF;}
            void set64(uint64_t val) {m_value = val;}

            uint64_t val64() const {return m_value;}
            uint32_t val32() const {return static_cast<uint32_t>(m_value & 0xFFFFFFFF);}
            uint16_t val16() const {return static_cast<uint16_t>(m_value & 0xFFFF);}
            uint8_t val8() const {return static_cast<uint8_t>(m_value & 0xFF);}

            void LoadValue(std::shared_ptr<Value> val) {
                if (val->isBoolean()) {
                    m_value = val->asBoolean() ? 1 : 0;
                } else if (val->isNumber()) {
                    m_value = static_cast<uint64_t>(val->asNumber());
                } else if (val->isString()) {
                    VERIFY_NOT_REACHED();
                } else {
                    m_value = 0;
                }
            }
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
            ProgramCounter() : Register(vm, 0xFE) {};
            ~ProgramCounter() override = default;

            bool IsInUse() const override {return true;}
            uint64_t get_address() const {return m_value;}
            void advance() {m_value++;}
        };
        class Accumulator final : public Register {
        public:
            Accumulator(VM& vm) : Register(vm, 0) {};
            ~Accumulator() override = default;

            bool LoadValue(Value& val) {

            }

            bool IsInUse() const override {return true;}
            void Reset() {m_value = 0;}
            void add_to_value(Register*& reg) {m_value += reg->val64();}
            void subtract_from_value(Register*& reg) {m_value -= reg->val64();}
            void multiply_with_value(Register*& reg) {m_value *= reg->val64();}
            void divide_with_value(Register*& reg) {m_value /= reg->val64();}
            void xor_with_value(Register*& reg) {m_value ^= reg->val64();}
            void and_with_value(Register*& reg) {m_value &= reg->val64();}
            void or_with_value(Register*& reg) {m_value |= reg->val64();}
            void negate_value() {m_value = -m_value;}
            void modulo_with_value(Register*& reg) {m_value %= reg->val64();}
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
