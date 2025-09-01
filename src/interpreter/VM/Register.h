#pragma once
#include <cstdint>
#include <cstdlib>
#include <ctime>

namespace js {
    namespace Interpreter {
        class Register {
        private:
            //for allocating specific register
            explicit Register(uint8_t reg_id): m_id(reg_id), in_use(true) {}

            //for allocating random register
            Register() : m_id(0), in_use(true) {
                srand(time(NULL));
                m_id = rand() % 256;
            }
        public:
            Register(const Register&) = default;
            Register& operator=(const Register&) = default;
            Register(Register&&) = delete;

            ~Register() = default;

            static Register* CreateWithRandomId() {
                return new Register();
            }
            static Register* CreateWithSpecificId(uint8_t reg_id) {
                return new Register(reg_id);
            }
            void SetValue(uint64_t val) {m_value = val;}
            uint64_t GetValue() const {return m_value;}
            
            virtual uint8_t Id() const {return m_id;}
            virtual bool IsInUse() const {return in_use;}
            void SetInUse(bool use) {in_use = use;}
        private:
            uint64_t m_value {0};
            uint8_t m_id {0};
            bool in_use {false};


        };
        class ProgramCounter {
        private:
            explicit ProgramCounter(uint32_t addr): m_address(addr) {}
        public:
            ProgramCounter(const ProgramCounter&) = delete;
            ProgramCounter& operator=(const ProgramCounter&) = delete;
            ProgramCounter(ProgramCounter&&) = delete;
            ~ProgramCounter() = default;
            static ProgramCounter* Create(uint32_t addr) {
                return new ProgramCounter(addr);
            }
            bool IsInUse() const override {return true;}

            void SetAddress(uint32_t addr) {m_address = addr;}
            uint32_t GetAddress() const {return m_address;}
        private:
            uint32_t m_address {0};

        };
        class Accumulator : public Register {

        };

    }
}
