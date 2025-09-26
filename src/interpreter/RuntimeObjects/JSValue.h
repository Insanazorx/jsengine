#pragma once
#include <iostream>
#include <string>
#include <variant>
#include "RuntimeObject.h"


namespace js {
    namespace Interpreter {
        class JSObject;


        class JSValue {
        public:
            static constexpr uint64_t TAG_SHIFT = 48;
            static constexpr uint64_t PAYLOAD_MASK = 0x0000FFFFFFFFFFFFFF;
            static constexpr uint64_t TAG_MASK = 0xFFFF000000000000;

            static constexpr uint64_t PRIMITIVE_TAG = 0x7FF8;

            static constexpr uint64_t UNDEFINED_TAG = 0b000 | PRIMITIVE_TAG;
            static constexpr uint64_t NULL_TAG = 0b001 | PRIMITIVE_TAG;
            static constexpr uint64_t BOOLEAN_TAG = 0b010 | PRIMITIVE_TAG;
            static constexpr uint64_t INT32_TAG = 0b011 | PRIMITIVE_TAG;
            static constexpr uint64_t OBJECT_TAG = 0b100 | PRIMITIVE_TAG;
            static constexpr uint64_t STRING_TAG = 0b101 | PRIMITIVE_TAG;
            static constexpr uint64_t BIGINT_TAG = 0b110 | PRIMITIVE_TAG;

            union NaN_Box{
                NaN_Box() : raw(UNDEFINED_TAG << TAG_SHIFT) {}
                explicit NaN_Box(double d) : as_double(d) {}
                explicit NaN_Box(int32_t int32_val) : raw(static_cast<uint64_t>(int32_val) | INT32_TAG << TAG_SHIFT) {}
                explicit NaN_Box(bool bool_val) : raw(bool_val ? static_cast<uint64_t>(1) | BOOLEAN_TAG << TAG_SHIFT
                    : static_cast<uint64_t>(0) | BOOLEAN_TAG << TAG_SHIFT ) {}
                explicit NaN_Box(std::nullptr_t) : raw(NULL_TAG << TAG_SHIFT) {}
                explicit NaN_Box(JSObject* obj) : raw((reinterpret_cast<uint64_t>(obj) & PAYLOAD_MASK) | OBJECT_TAG << TAG_SHIFT) {}
                explicit NaN_Box(const std::string& str) : raw((reinterpret_cast<uint64_t>(new std::string(str)) & PAYLOAD_MASK) | STRING_TAG << TAG_SHIFT) {}

                uint64_t raw;
                struct {
                    uint64_t payload : 48;
                    uint64_t tag : 16;
                };
                double as_double;
            } m_val {};


            JSValue() = default;
            explicit JSValue(int32_t int_value)
            : m_val(int_value) {}
            explicit JSValue(bool bool_value)
            : m_val(bool_value) {}
            explicit JSValue(std::nullptr_t)
            : m_val(nullptr) {}
            explicit JSValue(JSObject* obj)
            : m_val(obj) {}
            explicit JSValue(const std::string& str)
            : m_val(str) {}
            explicit JSValue(const char* cstr)
            : m_val(std::string(cstr)) {}
            explicit JSValue(double double_value)
            : m_val(double_value) {}

            bool isUndefined() const { return m_val.tag == UNDEFINED_TAG; }
            bool isNull() const { return m_val.tag == NULL_TAG; }
            bool isBoolean() const { return m_val.tag == BOOLEAN_TAG; }
            bool isInt32() const { return m_val.tag == INT32_TAG; }
            bool isObject() const { return m_val.tag == OBJECT_TAG; }
            bool isString() const { return m_val.tag == STRING_TAG; }
            bool isBigInt() const { return m_val.tag == BIGINT_TAG; }
            bool isNumber() const { return (m_val.tag & PRIMITIVE_TAG) != PRIMITIVE_TAG; }


            double asNumber() const {
                if (!isNumber()) {
                    throw std::runtime_error("JSValue is not a Number");
                }
                return m_val.as_double;
            }
            bool asBoolean() const {
                if (!isBoolean()) {
                    throw std::runtime_error("JSValue is not a Boolean");
                }
                return m_val.payload != 0;
            }
            JSObject* asObject() const {
                if (!isObject()) {
                    throw std::runtime_error("JSValue is not an Object");
                }
                return reinterpret_cast<JSObject*>(m_val.payload);
            }
            std::string asString() const {
                if (!isString()) {
                    throw std::runtime_error("JSValue is not a String");
                }
                return *reinterpret_cast<std::string*>(m_val.payload);
            }
            int32_t asInt32() const {
                if (!isInt32()) {
                    throw std::runtime_error("JSValue is not an Int32");
                }
                return static_cast<int32_t>(m_val.payload);
            }

            std::string toString() const {
                if (isUndefined()) return "undefined";
                if (isNull()) return "null";
                if (isBoolean()) return asBoolean() ? "true" : "false";
                if (isInt32()) return std::to_string(asInt32());
                if (isNumber()) return std::to_string(asNumber());
                if (isString()) return asString();
                if (isObject()) return "[object Object]";
                VERIFY_NOT_REACHED();
            }


            JSValue(const JSValue&) = default;
            JSValue& operator=(const JSValue&) = default;

        };

    } // namespace Interpreter
} // namespace js