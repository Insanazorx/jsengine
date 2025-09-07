#pragma once
#include <cstdint>
#include <string>
#include <unordered_map>

namespace js {
    namespace Interpreter {

        enum class ARG_TYPE {
            REGISTER,
            IMMEDIATE,
            SLOT,
            OBJECT_DESCRIPTOR,
            NAME_INDEX
        };

        using Reg = uint8_t;
        using Imm64 = uint64_t;
        using Imm32 = uint32_t;
        using Imm16 = uint16_t;
        using Imm8 = uint8_t;
        using Slot = uint8_t;
        using Object_Descriptor = uint16_t;
        using Name_Index = uint16_t;


        //X(name, code, arg_length, ...)
#define X_FOR_BYTECODES_WITH_ENUM_ARGS(X)                                                                       \
    X(LOAD_ACCUMULATOR_REG, 0x01, 2, ARG_TYPE::REGISTER)                                                        \
    X(LOAD_ACCUMULATOR_IMM64, 0x02, 9, ARG_TYPE::IMMEDIATE)                                                     \
    X(LOAD_ACCUMULATOR_IMM32, 0x3, 5, ARG_TYPE::IMMEDIATE)                                                      \
    X(LOAD_ACCUMULATOR_IMM16, 0x4, 3, ARG_TYPE::IMMEDIATE)                                                      \
    X(LOAD_ACCUMULATOR_IMM8, 0x5, 2, ARG_TYPE::IMMEDIATE)                                                       \
    X(STORE_ACCUMULATOR, 0x06, 2,ARG_TYPE::REGISTER)                                                            \
    X(ADD, 0x07, 2,ARG_TYPE::REGISTER)                                                                          \
    X(SUBTRACT, 0x08, 2,ARG_TYPE::REGISTER)                                                                     \
    X(MULTIPLY, 0x09, 2,ARG_TYPE::REGISTER)                                                                     \
    X(DIVIDE, 0x0A, 2,ARG_TYPE::REGISTER)                                                                       \
    X(JUMP, 0x0B, 2,ARG_TYPE::IMMEDIATE)                                                                        \
    X(COMPARE, 0x0C, 2,ARG_TYPE::REGISTER)                                                                      \
    X(TEST, 0x0D, 2,ARG_TYPE::REGISTER)                                                                         \
    X(JUMP_IF_EQUAL,0x0E, 5,ARG_TYPE::REGISTER)                                                                 \
    X(JUMP_IF_ZERO, 0x0F, 5,ARG_TYPE::REGISTER)                                                                 \
    X(JUMP_UNLESS_ZERO,0x10, 5,ARG_TYPE::REGISTER)                                                              \
    X(CALL_FUNCTION, 0x11, 3,ARG_TYPE::NAME_INDEX)                                                              \
    X(LOAD_NAMED_PROPERTY, 0x12, 5, ARG_TYPE::OBJECT_DESCRIPTOR, ARG_TYPE::NAME_INDEX)                          \
    X(LOAD_KEYED_PROPERTY, 0x13, 4,ARG_TYPE::OBJECT_DESCRIPTOR, ARG_TYPE::REGISTER)                             \
    X(LOAD_INDEXED_PROPERTY, 0x14, 4, ARG_TYPE::OBJECT_DESCRIPTOR, ARG_TYPE::SLOT)                              \
    X(CREATE_OBJECT, 0x15, 3,ARG_TYPE::NAME_INDEX)                                                              \
    X(CREATE_CONTEXT, 0x16, 1)                                                                                  \
    X(NOP, 0x17, 1)                                                                                             \
    X(RETURN, 0x18, 1)                                                                                          \
    X(XOR, 0x19, 2, ARG_TYPE::REGISTER)                                                                         \
    X(AND, 0x1A, 2, ARG_TYPE::REGISTER)                                                                         \
    X(OR, 0x1B, 2, ARG_TYPE::REGISTER)                                                                          \
    X(NEGATE, 0x1C, 1)                                                                                          \
    X(MODULO, 0x1D, 2,ARG_TYPE::REGISTER)                                                                       \
    X(SHIFT_LEFT, 0x1E, 2,ARG_TYPE::REGISTER)                                                                   \
    X(SHIFT_RIGHT, 0x1F, 2,ARG_TYPE::REGISTER)                                                                  \
    X(CREATE_FUNCTION, 0x20, 3,ARG_TYPE::NAME_INDEX)                                                            \
    X(CREATE_ARRAY, 0x21, 3,ARG_TYPE::NAME_INDEX)                                                               \
    X(SAVE_STATE, 0x22, 1)                                                                                      \
    X(RESTORE_STATE, 0x23, 1)                                                                                   \
    X(DEBUG_BREAK, 0x24, 1)                                                                                     \
    X(DEBUG_PRINT, 0x25, 2, ARG_TYPE::REGISTER)                                                                 \
    X(HALT, 0xFF, 1)

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
    X(JUMP_IF_EQUAL,0x0E, 5, Reg)                                                                               \
    X(JUMP_IF_ZERO, 0x0F, 5, Reg)                                                                               \
    X(JUMP_UNLESS_ZERO,0x10, 5, Reg)                                                                            \
    X(CALL_FUNCTION, 0x11, 3, Name_Index)                                                                       \
    X(LOAD_NAMED_PROPERTY, 0x12, 5, Object_Descriptor, Name_Index)                                              \
    X(LOAD_KEYED_PROPERTY, 0x13, 4, Object_Descriptor, Reg)                                                     \
    X(LOAD_INDEXED_PROPERTY, 0x14, 4, Object_Descriptor, Slot)                                                  \
    X(CREATE_OBJECT, 0x15, 3, Name_Index)                                                                       \
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
    X(CREATE_FUNCTION, 0x20, 3, Name_Index)                                                                             \
    X(CREATE_ARRAY, 0x21, 3, Name_Index)                                                                        \
    X(SAVE_STATE, 0x22, 1)                                                                                      \
    X(RESTORE_STATE, 0x23, 1)                                                                                   \
    X(DEBUG_BREAK, 0x24, 1)                                                                                     \
    X(DEBUG_PRINT, 0x25, 2, Reg)                                                                                \
    X(HALT, 0xFF, 1)

#define ENUMERATE_BYTECODES(name, code, length...) name = code,
#define MAKE_UNORDERED_MAP_AS_STRING_FINDER(name, code, length, ...) {code, #name},
#define MAKE_UNORDERED_MAP_AS_CODE_FINDER(name, code, length, ...) {name, code},
#define GET_BYTECODE_LENGTH(name, code, length, ...) case name: return length;

#define DECLARE_HANDLER(name, ...) inline __attribute__((always_inline)) void name##_HANDLER (__VA_ARGS__)
#define HANDLER_PROTOTYPE(name, code, length, ...) inline __attribute__((always_inline)) void name##_HANDLER (__VA_ARGS__);
#define INVOKE_HANDLER(name, ...) name##_HANDLER (__VA_ARGS__);


        enum Opcode : uint8_t {
            X_FOR_BYTECODES_WITH_ENUM_ARGS(ENUMERATE_BYTECODES)
        };
        inline const std::unordered_map<uint8_t, std::string> opcode_to_string_list {
            X_FOR_BYTECODES_WITH_ENUM_ARGS(MAKE_UNORDERED_MAP_AS_STRING_FINDER)
        };

        inline int get_bytecode_length(Opcode opcode) {
            switch (opcode) {
                X_FOR_BYTECODES_WITH_ENUM_ARGS(GET_BYTECODE_LENGTH)
            default:
                return -1;
            }
            }

        }; // namespace Interpreter
    }
