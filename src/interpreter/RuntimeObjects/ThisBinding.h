#pragma once
#include "JSObject.h"
#include "RuntimeObject.h"

namespace js {
    namespace Interpreter {
        class ThisBinding : public RuntimeObject {
        public:
            ThisBinding() = default;
            ~ThisBinding() override = default;
        private:
            JSObject* m_this_value;

        };
    }
}