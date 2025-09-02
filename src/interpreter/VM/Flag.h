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
        };

        struct FlagSet : VMObject {
            FlagSet(VM& vm) : VMObject(vm) {}
            ~FlagSet() override = default;

            bool FLAG_ZERO : 1 {0};
            bool FLAG_EQUAL : 1 {0};
            bool FLAG_CARRY : 1 {0};
            bool FLAG_OVERFLOW : 1 {0};
            bool FLAG_NEGATIVE : 1 {0};
            bool FLAG_RESERVED : 3 {0};
        };
    }
}
