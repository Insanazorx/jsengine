#pragma once
#include <string>

namespace js {
    namespace Interpreter {
        class RuntimeObject {
        public:
            RuntimeObject() = default;
            virtual ~RuntimeObject() = default;
            virtual bool IsRuntimeObject() const { return true; }
            int id {0};

        };
    }; // namespace Interpreter
}
