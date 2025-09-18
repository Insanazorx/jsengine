#pragma once
#include <cstdint>
#include <string>
#include <unordered_map>

namespace js {
    namespace Interpreter {

        using Reg = uint8_t;
        using Imm64 = uint64_t;
        using Imm32 = uint32_t;
        using Imm16 = uint16_t;
        using Imm8 = uint8_t;
        using Slot = uint8_t;
        using Name_Index = uint16_t;


        //X(name, code, arg_length, ...)
#define X_FOR_BYTECODES_WITH_TYPED_ARGS(X)                                                                      \
    X(LOAD_ACCUMULATOR_REG, 0x01, 2, Reg)                                                                       \
    X(LOAD_ACCUMULATOR_IMM64, 0x02, 9, Imm64)                                                                   \
    X(LOAD_ACCUMULATOR_IMM32, 0x3, 5, Imm32)                                                                    \
    X(LOAD_ACCUMULATOR_IMM16, 0x4, 3, Imm16)                                                                    \
    X(LOAD_ACCUMULATOR_IMM8, 0x5, 2, Imm8)                                                                      \
    X(STORE_ACCUMULATOR, 0x06, 2, Reg)                                                                          \
    X(ADD, 0x07, 2, Reg)                                                                                        \
    X(SUBTRACT, 0x08, 2, Reg)                                                                                   \
    X(MULTIPLY, 0x09, 2, Reg)                                                                                   \
    X(DIVIDE, 0x0A, 2, Reg)                                                                                     \
    X(JUMP, 0x0B, 2, Reg)                                                                                       \
    X(COMPARE, 0x0C, 2, Reg)                                                                                    \
    X(TEST, 0x0D, 2, Reg)                                                                                       \
    X(JUMP_IF_EQUAL,0x0E, 2, Reg)                                                                               \
    X(JUMP_IF_ZERO, 0x0F, 2, Reg)                                                                               \
    X(JUMP_IFNOT_ZERO, 0x10, 2, Reg)                                                                            \
    X(CALL_FUNCTION, 0x11, 3, Name_Index)                                                                       \
    X(LOAD_NAMED_PROPERTY, 0x12, 3, Name_Index)                                                                 \
    X(LOAD_KEYED_PROPERTY, 0x13, 2, Reg)                                                                        \
    X(LOAD_INDEXED_PROPERTY, 0x14, 2, Slot)                                                                     \
    X(CREATE_OBJECT, 0x15, 1)                                                                                   \
    X(CREATE_CONTEXT, 0x16, 1)                                                                                  \
    X(NOP, 0x17, 1)                                                                                             \
    X(RETURN, 0x18, 1)                                                                                          \
    X(XOR, 0x19, 2, Reg)                                                                                        \
    X(AND, 0x1A, 2, Reg)                                                                                        \
    X(OR, 0x1B, 2, Reg)                                                                                         \
    X(NEGATE, 0x1C, 1)                                                                                          \
    X(MODULO, 0x1D, 2,Reg)                                                                                      \
    X(SHIFT_LEFT, 0x1E, 2,Reg)                                                                                  \
    X(SHIFT_RIGHT, 0x1F, 2,Reg)                                                                                 \
    X(CREATE_FUNCTION, 0x20, 1)                                                                                 \
    X(CREATE_ARRAY, 0x21, 1)                                                                                    \
    X(SAVE_STATE, 0x22, 1)                                                                                      \
    X(RESTORE_STATE, 0x23, 1)                                                                                   \
    X(DEBUG_BREAK, 0x24, 1)                                                                                     \
    X(DEBUG_PRINT, 0x25, 2, Reg)                                                                                \
    X(JUMP_IFNOT_EQUAL, 0X26, 2, Reg)                                                                           \
    X(JUMP_IF_GT, 0x27, 2, Reg)                                                                                 \
    X(JUMP_IF_LT, 0x28, 2, Reg)                                                                                 \
    X(JUMP_IF_GTE, 0x29, 2, Reg)                                                                                \
    X(JUMP_IF_LTE, 0x2A, 2, Reg)                                                                                \
    X(CLEAR_FLAGS, 0x2B, 1)                                                                                     \
    X(SET_ACC_IS_OD_FLAG, 0x2C, 1)                                                             \
    X(HALT, 0xFF, 1)

#define ENUMERATE_BYTECODES(name, code, length...) name = code,
#define MAKE_UNORDERED_MAP_AS_STRING_FINDER(name, code, length, ...) {code, #name},
#define MAKE_UNORDERED_MAP_AS_CODE_FINDER(name, code, length, ...) {name, code},
#define GET_BYTECODE_LENGTH(name, code, length, ...) case name: return length;

#define DECLARE_HANDLER(name, ...) inline __attribute__((always_inline)) void VM::name##_HANDLER (__VA_ARGS__)
#define HANDLER_PROTOTYPE(name, code, length, ...) inline __attribute__((always_inline)) void name##_HANDLER (__VA_ARGS__);
#define INVOKE_HANDLER(name, ...) name##_HANDLER (__VA_ARGS__);

        enum Opcode : uint8_t {
            X_FOR_BYTECODES_WITH_TYPED_ARGS(ENUMERATE_BYTECODES)
        };
        inline const std::unordered_map<uint8_t, std::string> opcode_to_string_list {
            X_FOR_BYTECODES_WITH_TYPED_ARGS(MAKE_UNORDERED_MAP_AS_STRING_FINDER)
        };

        inline int get_bytecode_length(Opcode opcode) {
            switch (opcode) {
                X_FOR_BYTECODES_WITH_TYPED_ARGS(GET_BYTECODE_LENGTH)
            default:
                return -1;
            }
            }

        }; // namespace Interpreter
    }
