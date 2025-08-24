#pragma once


#include "RuntimeObject.h"
#include "Value.h"

namespace js {
    namespace Interpreter {

        class ExecutionContext : public RuntimeObject{
        private:
            ExecutionContext() = default;

        public:

            static ExecutionContext* Create() {
                return new ExecutionContext();
            }

            ~ExecutionContext() override = default;



        };//class ExecutionContext

        class Environment : public RuntimeObject {
        private:
            Environment() = default;
        public:
            static Environment* Create() {
                return new Environment();
            }
            ~Environment() override = default;


        private:
            std::vector<Value> m_variables;
            Environment* m_outerEnvironment {nullptr};
            Environment* m_innerEnvironment {nullptr};

        };

    }// namespace Interpreter
}// namespace js
