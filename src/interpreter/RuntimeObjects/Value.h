#pragma once
#include <iostream>
#include <string>
#include <variant>
#include "RuntimeObject.h"


namespace js {
    namespace Interpreter {
        class JSObject;
        enum class ValueType {
            Undefined,
            Null,
            Boolean,
            Number,
            BigInt,
            String,
            Object
        };

        class Value : public RuntimeObject {
        public:
            using Variant = std::variant<
                std::monostate,               // undefined
                std::nullptr_t,               // null
                bool,                         // boolean
                double,                       // number
                uint64_t,                     // bigint
                std::string,                  // string
                std::shared_ptr<JSObject>     // object
            >;

        private:
            Variant m_data;

        public:

        Value() : m_data(std::monostate{}) {}
        Value(std::nullptr_t) : m_data(nullptr) {}
        Value(bool b) : m_data(b) {}
        Value(double n) : m_data(n) {}
        Value(const std::string& s) : m_data(s) {}
        Value(const char* s) : m_data(std::string(s)) {}
        Value(std::shared_ptr<JSObject> obj) : m_data(obj) {}


            ValueType type() const {
                if (std::holds_alternative<std::monostate>(m_data)) return ValueType::Undefined;
                if (std::holds_alternative<std::nullptr_t>(m_data)) return ValueType::Null;
                if (std::holds_alternative<bool>(m_data)) return ValueType::Boolean;
                if (std::holds_alternative<double>(m_data)) return ValueType::Number;
                if (std::holds_alternative<std::string>(m_data)) return ValueType::String;
                if (std::holds_alternative<uint64_t>(m_data)) return ValueType::BigInt;
                return ValueType::Object;
            }


    bool isUndefined() const { return type() == ValueType::Undefined; }
    bool isNull() const { return type() == ValueType::Null; }
    bool isBoolean() const { return type() == ValueType::Boolean; }
    bool isNumber() const { return type() == ValueType::Number; }
    bool isString() const { return type() == ValueType::String; }
    bool isObject() const { return type() == ValueType::Object; }
            bool isBigInt() const { return type() == ValueType::BigInt; }


    double asNumber() const { return std::get<double>(m_data); }
    const std::string& asString() const { return std::get<std::string>(m_data); }
    bool asBoolean() const { return std::get<bool>(m_data); }
    std::shared_ptr<JSObject> asObject() const { return std::get<std::shared_ptr<JSObject>>(m_data); }


    void print() const {
        switch (type()) {
            case ValueType::Undefined:
                std::cout << "undefined";
                break;
            case ValueType::Null:
                std::cout << "null";
                break;
            case ValueType::Boolean:
                std::cout << (asBoolean() ? "true" : "false");
                break;
            case ValueType::Number:
                std::cout << asNumber();
                break;
            case ValueType::String:
                std::cout << '"' << asString() << '"';
                break;
            case ValueType::Object:
                std::cout << "[object Object]";
                break;
        }
    }
};

    } // namespace Interpreter
} // namespace js