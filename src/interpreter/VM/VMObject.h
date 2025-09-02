#pragma once

namespace js {
    namespace Interpreter {
        class VM;

        class VMObject {
        public:
            explicit VMObject(VM& vm_ref) : vm(vm_ref) {}
            public:
            virtual ~VMObject() = default;

            virtual bool is_execution_state() const { return false; }
            virtual bool is_register() const { return false; }
            virtual bool is_flag_set() const { return false; }
            virtual bool is_stack() const { return false; }

        protected:
            VM& vm;


        };
    }
}