#pragma once
#include <string>

namespace js {
    namespace Interpreter {
        class RuntimeObject {
        public:
            RuntimeObject() = default;
            virtual ~RuntimeObject() = default;


            std::string& name() {
                return m_name;
            }
        private:
            std::string m_name {"Object"};
        };
    }; // namespace Interpreter
}
