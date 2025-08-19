#pragma once
#include <string>

#include "JSObject.h"

namespace js {
    namespace Interpreter {

        enum class Type {
            Undefined,
            Null,
            Boolean,
            Number,
            String,
            Object
        };

        class Value : public RuntimeObject {
        public:
            Value() = default;
            virtual ~Value() = default;

            virtual bool IsUndefined() const { return false; }
            virtual bool IsNull() const { return false; }
            virtual bool IsBoolean() const { return false; }
            virtual bool IsNumber() const { return false; }
            virtual bool IsString() const { return false; }
            virtual bool IsObject() const { return false; }

        private:
            Type m_type {Type::Undefined};
            union {
                uint32_t m_smi_value;
                std::nullptr_t m_null_value;
                bool m_boolean_value;
                double m_number_value;
                std::string* m_string_value;
            } m_value {.m_smi_value = 0};
        };

    } // namespace Interpreter
} // namespace js