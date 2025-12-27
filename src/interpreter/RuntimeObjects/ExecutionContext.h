#pragma once


#include "Environment.h"
#include "RuntimeObject.h"
#include "Binding.h"
#include "JSValue.h"

namespace js {
    namespace Interpreter {
        class ExecutionContext {
        public:
            ExecutionContext() = default;
            ~ExecutionContext() = default;

            void allocate_environment(LexicalEnvironment* env) {
                m_outer_environment = m_current_environment;
                m_current_environment = env;
            }
            LexicalEnvironment* current_environment() { return m_current_environment; }
        private:
            LexicalEnvironment* m_current_environment {nullptr};
            LexicalEnvironment* m_outer_environment {nullptr};
            bool m_strict_mode {false};



            
        };//class ExecutionContext
    }// namespace Interpreter
}// namespace js
