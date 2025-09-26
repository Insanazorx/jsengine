#pragma once


#include "Environment.h"
#include "RuntimeObject.h"
#include "ThisBinding.h"
#include "JSValue.h"

namespace js {
    namespace Interpreter {
        class ExecutionContext : public RuntimeObject{
        public:
            ExecutionContext() = default;
            ~ExecutionContext() override = default;

            void allocate_environment(LexicalEnvironment* env) {
                m_outer_environment = m_current_environment;
                m_current_environment = env;
            }
            LexicalEnvironment* current_environment() { return m_current_environment; }
        private:
            ThisBinding* m_this_binding;
            LexicalEnvironment* m_current_environment {nullptr};
            LexicalEnvironment* m_outer_environment {nullptr};
            bool m_strict_mode {false};



            
        };//class ExecutionContext
    }// namespace Interpreter
}// namespace js
