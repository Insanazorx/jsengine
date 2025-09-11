#pragma once


#include "Environment.h"
#include "RuntimeObject.h"
#include "ThisBinding.h"
#include "Value.h"

namespace js {
    namespace Interpreter {
        class ExecutionContext : public RuntimeObject{
        public:
            ExecutionContext() = default;
            ~ExecutionContext() override = default;

            void allocate_environment(Environment* env) {
                m_outer_environment = m_current_environment;
                m_current_environment = env;
            }
            Environment* current_environment() { return m_current_environment; }
        private:
            ThisBinding m_this_binding;
            Environment* m_current_environment {nullptr};
            Environment* m_outer_environment {nullptr};
            bool m_strict_mode {false};



            
        };//class ExecutionContext
    }// namespace Interpreter
}// namespace js
