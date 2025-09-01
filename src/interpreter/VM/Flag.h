#pragma once
#include <cstdint>

namespace js {
    namespace Interpreter {
        enum class Flag : uint8_t {
            ZERO = 1 << 0,
            EQUAL = 1 << 1,
            CARRY = 1 << 2,
            OVERFLOW = 1 << 3,
            NEGATIVE = 1 << 4,
        };

        struct FlagSet {
            bool ZERO : 1 {0};
            bool EQUAL : 1 {0};
            bool CARRY : 1 {0};
            bool OVERFLOW : 1 {0};
            bool NEGATIVE : 1 {0};
            bool RESERVED : 3 {0};
        };
    }
}
