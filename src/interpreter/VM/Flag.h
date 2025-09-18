#pragma once
#include <cstdint>

#include "VMObject.h"

namespace js {
    namespace Interpreter {
        enum class Flag : uint8_t {
            F_ZERO = 1 << 0,
            F_EQUAL = 1 << 1,
            F_CARRY = 1 << 2,
            F_OVERFLOW = 1 << 3,
            F_NEGATIVE = 1 << 4,
            F_GT = 1 << 5,
            F_LT = 1 << 6,
            F_IS_ACC_OD = 1 << 7
        };

        struct FlagSet : VMObject {
            FlagSet(VM& vm) : VMObject(vm) {}
            ~FlagSet() override = default;

            union {
                struct {
                    bool FLAG_ZERO : 1 {0};
                    bool FLAG_EQUAL : 1 {0};
                    bool FLAG_CARRY : 1 {0};
                    bool FLAG_OVERFLOW : 1 {0};
                    bool FLAG_NEGATIVE : 1 {0};
                    bool FLAG_GT : 1 {0};
                    bool FLAG_LT : 1 {0};
                    bool FLAG_IS_ACC_OD : 1 {0};
                } flags;
                uint8_t all_flags {0};
            }flags_union;
        };
    }
}
