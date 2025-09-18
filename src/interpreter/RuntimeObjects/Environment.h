#pragma once
#include <unordered_map>
#include <vector>

#include "JSObject.h"
#include "RuntimeObject.h"
#include "Value.h"

namespace js {
    namespace Interpreter {


        class LexicalEnvironment : public RuntimeObject {

        public:
            LexicalEnvironment() {}
            static LexicalEnvironment* Create() {
                return new LexicalEnvironment();
            }
            ~LexicalEnvironment() override = default;
            uint16_t create_object(JSObject* obj, const std::string& name) {
                Name_Index od = m_js_objects.size();
                m_js_objects[od] = obj;
                m_all_names[name] = od;
                return od;
            }

            JSObject* get_object_by_descriptor(Name_Index od) {
                auto it = m_js_objects.find(od);
                if (it != m_js_objects.end()) {
                    return it->second;
                }
                return nullptr;
            }

            int all_names_count() const {
                return m_all_names.size();
            }

            int all_values_count() const {
                return m_all_values.size();
            }

            int js_objects_count() const {
                return m_js_objects.size();
            }


        protected:
            uint16_t m_scope_depth {0};
            std::unordered_map <std::string, Name_Index> m_all_names;
            std::unordered_map<Name_Index, JSObject*> m_js_objects;
            std::unordered_map<Name_Index, Value> m_all_values;

        };


    }
}
