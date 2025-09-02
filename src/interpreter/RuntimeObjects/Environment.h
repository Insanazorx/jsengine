#pragma once
#include <vector>

#include "JSObject.h"
#include "RuntimeObject.h"
#include "Value.h"

namespace js {
    namespace Interpreter {
        struct Variable {
            uint16_t id;
            std::string name;
            Value value;
            JSObject* object {nullptr};
        };

        class Environment : public RuntimeObject {
        public:
            Environment() = default;
            static Environment* Create() {
                return new Environment();
            }
            ~Environment() override = default;


        protected:
            uint16_t m_scope_depth {0};
            std::vector<Variable> m_variables;
        };


    }
}
