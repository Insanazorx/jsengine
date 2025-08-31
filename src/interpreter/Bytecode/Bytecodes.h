#pragma once
#include <cstdint>
#include <string>

namespace js {
    namespace Interpreter {
        class Register;
        class Value;

        enum class ARG_TYPE{
            REGISTER,
            IMMEDIATE,
            ADDRESS,
            OBJECT_DESCRIPTOR,
            SLOT,
            NAME
        };
        using Slot = uint8_t;
        using Address = uint32_t;
        using Object_Descriptor = uint16_t;
        using Name = std::string;

#define X_BYTECODES(X)                                                                                  \
    X(LOAD_ACCUMULATOR_REG, 0x01 ,ARG_TYPE::REGISTER)                                                   \
    X(LOAD_ACCUMULATOR_IMM, 0x02, ARG_TYPE::IMMEDIATE)                                                  \
    X(STORE_ACCUMULATOR, 0x03, ARG_TYPE::REGISTER)                                                      \
    X(ADD, 0x04, ARG_TYPE::REGISTER)                                                                    \
    X(SUBTRACT, 0x05, ARG_TYPE::REGISTER)                                                               \
    X(MULTIPLY, 0x06, ARG_TYPE::REGISTER)                                                               \
    X(DIVIDE, 0x07, ARG_TYPE::REGISTER)                                                                 \
    X(JUMP, 0x08, ARG_TYPE::ADDRESS)                                                                    \
    X(COMPARE, 0x09)                                                                                    \
    X(TEST, 0x0A)                                                                                       \
    X(JUMP_IF_EQUAL, 0x0B, ARG_TYPE::ADDRESS)                                                           \
    X(JUMP_IF_ZERO, 0x0C, ARG_TYPE::ADDRESS)                                                            \
    X(JUMP_UNLESS_ZERO,0xD, ARG_TYPE::ADDRESS)                                                          \
    X(CALL_FUNCTION, 0x0E, ARG_TYPE::NAME)                                                              \
    X(LOAD_NAMED_PROPERTY, 0x0F, ARG_TYPE::OBJECT_DESCRIPTOR, ARG_TYPE::NAME)                           \
    X(LOAD_KEYED_PROPERTY, 0x10, ARG_TYPE::OBJECT_DESCRIPTOR, ARG_TYPE::REGISTER)                       \
    X(LOAD_INDEXED_PROPERTY, 0x11, ARG_TYPE::OBJECT_DESCRIPTOR, ARG_TYPE::SLOT)                         \
    X(CREATE_OBJECT, 0x12)                                                                              \
    X(CREATE_CONTEXT, 0x13)                                                                             \
    X(NOP, 0x14)                                                                                        \
    X(RETURN, 0x15)

#define ENUMERATE_BYTECODES(name, code, ...) name = code,

#define DECLARE_HANDLER(name, ...) void BytecodeGenerator:: ##name##_HANDLER (__VA_ARGS__)
#define HANDLER_PROTOTYPE(name, ...) void name##_HANDLER (__VA_ARGS__);

        enum class OpCode : uint8_t {
            X_BYTECODES(ENUMERATE_BYTECODES)
        };

        }; // namespace Interpreter
    }
