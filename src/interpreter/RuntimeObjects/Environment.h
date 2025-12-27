#pragma once
#include <unordered_map>
#include <vector>

#include "JSObject.h"
#include "RuntimeObject.h"
#include "JSValue.h"

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

            }

            JSObject* get_object_by_descriptor() {

                return nullptr;
            }



        protected:
            uint16_t m_scope_depth {0};
            std::unordered_map <std::string, Name_Index> m_all_names;
        };


    }
}
