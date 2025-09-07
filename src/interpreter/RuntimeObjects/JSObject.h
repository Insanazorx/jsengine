#pragma once
#include "RuntimeObject.h"
#include "Value.h"

namespace js {
    namespace Interpreter {
        class JSObject : public RuntimeObject {
        public:
            JSObject() = default;
            virtual ~JSObject() = default;

            virtual bool IsJSObject() const { return false; }
            virtual std::string ToString() const { return "[object Object]"; }

            // Additional methods for JSObject can be defined here
            virtual void SetProperty(const std::string& name, const Value& value) {}
            virtual Value& GetProperty(const std::string& name) const { return {};}
            virtual bool HasProperty(const std::string& name) const { return false; }
        private:
            JSObject* m_prototype {nullptr};
        }; // class JSObject

    } // namespace Interpreter
} // namespace js
