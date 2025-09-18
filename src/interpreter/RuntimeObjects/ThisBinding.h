#pragma once
#include "JSObject.h"

namespace js {
    namespace Interpreter {
        class ThisBinding : public JSObject {
        public:
            ThisBinding() = default;
            ~ThisBinding() override = default;
        private:
            JSObject* m_this_value;

        };
    }
}