#pragma once
#include "RuntimeObject.h"
#include "Value.h"

namespace js {
    namespace Interpreter {
        class JSObject : public RuntimeObject {
        public:
            JSObject() = default;
            virtual ~JSObject() = default;

            virtual bool IsJSObject() const { return true; }
            virtual std::string ToString() const { return "[object Object]"; }

            // Additional methods for JSObject can be defined here
            virtual void SetProperty(const std::string& name, const Value& value) {}
            virtual Value& GetProperty(const std::string& name) const { }
            virtual bool HasProperty(const std::string& name) const { return false; }

        }; // class JSObject

    } // namespace Interpreter
} // namespace js
