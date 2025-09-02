#pragma once
#include <vector>

#include "../RuntimeObjects/RuntimeObject.h"

namespace js {
    namespace Interpreter {
        class Stack : public VMObject{
        public:
            explicit Stack(VM& vm) : VMObject(vm) {}
            ~Stack() override = default;
            void push(const RuntimeObject& obj) {
                m_stack.push_back(obj);
            }
            RuntimeObject pop() {
                if (m_stack.empty()) {
                    throw std::runtime_error("Stack underflow: Attempt to pop from an empty stack.");
                }
                RuntimeObject obj = m_stack.back();
                m_stack.pop_back();
                return obj;
            }
            void save_state () {

            }
        private:
            std::vector<RuntimeObject> m_stack;
        };
    }
}
