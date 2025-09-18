#pragma once
#include "RuntimeObject.h"
#include "Value.h"

namespace js {
    namespace Interpreter {
        class JSObject : public RuntimeObject {
        protected:
            JSObject() = default;

        public:
            static JSObject* Create() {
                return new JSObject();
            }
            virtual ~JSObject() = default;

            virtual bool IsJSObject() const { return false; }
            virtual std::string ToString() const { return "[object Object]"; }

            // Additional methods for JSObject can be defined here

            virtual void SetProperty(Name_Index name_idx, std::shared_ptr<Value> value) {
                m_properties[name_idx] = value;
            }

            virtual std::shared_ptr<Value> GetPropertyByName(Name_Index name_idx){
                auto it = m_properties.find(name_idx);
                if (it != m_properties.end()) {
                    return it->second;
                }
                return std::move(std::make_shared<Value>());
            }

            virtual std::shared_ptr<Value> GetElementByIndex(int id) {
                return m_elements.at(id);
            }

            virtual bool HasProperty(Name_Index name_idx) {
                auto it = m_properties.find(name_idx);
                return it != m_properties.end();
            }
        protected:
            std::vector<std::shared_ptr<Value>> m_elements {};
            JSObject* m_prototype {nullptr};
            std::string m_name {};
            std::unordered_map<Name_Index, std::shared_ptr<Value>> m_properties {};

        }; // class JSObject

    } // namespace Interpreter
} // namespace js
