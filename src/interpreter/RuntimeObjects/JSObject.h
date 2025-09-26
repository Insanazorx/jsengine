#pragma once
#include "RuntimeObject.h"
#include "JSValue.h"

namespace js {
    namespace Interpreter {
        class JSObject : public RuntimeObject {
        protected:
            JSObject(std::string name) : m_name(std::move(name)) {}

        public:
            static JSObject* Create(std::string&& name) {
                return new JSObject(std::move(name));
            }
            virtual ~JSObject() = default;

            virtual bool IsJSObject() const { return false; }
            virtual std::string ToString() const { return "[object Object]"; }

            // Additional methods for JSObject can be defined here

            virtual void SetProperty(Name_Index name_idx, std::shared_ptr<JSValue> value) {
                m_properties[name_idx] = value;
            }

            virtual std::shared_ptr<JSValue> GetPropertyByName(Name_Index name_idx){
                auto it = m_properties.find(name_idx);
                if (it != m_properties.end()) {
                    return it->second;
                }
                return std::move(std::make_shared<JSValue>());
            }

            virtual std::shared_ptr<JSValue> GetElementByIndex(int id) {
                return m_elements.at(id);
            }

            virtual bool HasProperty(Name_Index name_idx) {
                auto it = m_properties.find(name_idx);
                return it != m_properties.end();
            }
        protected:
            std::vector<std::shared_ptr<JSValue>> m_elements {};
            JSObject* m_prototype {nullptr};
            std::string m_name {};
            std::unordered_map<Name_Index, std::shared_ptr<JSValue>> m_properties {};

        }; // class JSObject

    } // namespace Interpreter
} // namespace js
