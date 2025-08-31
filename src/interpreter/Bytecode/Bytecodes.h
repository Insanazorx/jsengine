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
#define GET_ARGS(name, code, ...) __VA_ARGS__

#define DECLARE_HANDLER(name, code ,...) void name##_HANDLER (__VA_ARGS__);

template <ARG_TYPE... T> struct ArgTypeToCType;
template <> struct ArgTypeToCType<ARG_TYPE::REGISTER> {using type = Register&;};
template <> struct ArgTypeToCType<ARG_TYPE::IMMEDIATE> {using type = Value&;};
template <> struct ArgTypeToCType<ARG_TYPE::ADDRESS> {using type = uint32_t;};
template <> struct ArgTypeToCType<ARG_TYPE::OBJECT_DESCRIPTOR> {using type = uint32_t;};
template <> struct ArgTypeToCType<ARG_TYPE::SLOT> {using type = uint8_t;};
        template <> struct ArgTypeToCType<ARG_TYPE::NAME> {using type = std::string&;};
        template <> struct ArgTypeToCType<> {using type = void;};

#define ARG_TO_CTYPE(arg) typename ArgTypeToCType<arg>::type
#define MAP_ARG_TO_CTYPE_1(a) ARG_TO_CTYPE(a)
#define MAP_ARG_TO_CTYPE_2(a, b) ARG_TO_CTYPE(a), ARG_TO_CTYPE(b)
#define MAP_ARG_TO_CTYPE_3(a, b, c) ARG_TO_CTYPE(a), ARG_TO_CTYPE(b), ARG_TO_CTYPE(c)
#define MAP_ARG_TO_CTYPE_4(a, b, c, d) ARG_TO_CTYPE(a), ARG_TO_CTYPE(b), ARG_TO_CTYPE(c), ARG_TO_CTYPE(d)
#define GET_ARG_COUNT(N, ...) \
         __VA_OPT__(1) __VA_ARGS__ 1 + GET_ARG_COUNT(__VA_ARGS__)

#define SELECT_MAP(N) MAP_ARG_TO_CTYPE_##N
#define EXPAND(x) x
#define HANDLER_PROTOTYPES(name, code, ...) DECLARE_HANDLER(name, code, EXPAND(SELECT_MAP(GET_ARG_COUNT(__VA_ARGS__))))

        enum class OpCode : uint8_t {
            X_BYTECODES(ENUMERATE_BYTECODES)
        };

        }; // namespace Interpreter
    }
