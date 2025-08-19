#pragma once
#include <string>

namespace js {
    namespace Interpreter {

        class RuntimeObject {
        public:
            RuntimeObject() = default;
            virtual ~RuntimeObject() = default;

        };
        std::string& name() const {
            return m_name;
        }

        std::string m_name {"Object"};

    } // namespace Interpreter
}
