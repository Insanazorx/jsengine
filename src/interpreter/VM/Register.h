#pragma once
#include <cstdint>

namespace js {
    namespace Interpreter {
        class Register {
        public:
            Register(std::uint8_t reg_id): id(reg_id), in_use(true) {}
            ~Register() = default;
            
            void SetValue(uint64_t val) {Value = val;}
            uint64_t GetValue() const {return Value;}
            
            std::uint8_t Id() const {return id;}
            bool IsInUse() const {return in_use;}
            void SetInUse(bool use) {in_use = use;}
        private:
            uint64_t Value {0};
            std::uint8_t id;
            bool in_use {false};

        };
    }
}
