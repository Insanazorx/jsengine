#pragma once
#include <string>
#include "Instruction.h"

namespace js {
    namespace Interpreter {

        class Label : public Instruction {
        private:
            explicit Label(std::string& name) : m_name(name) {}
        public:
            static Label* Create(std::string&& name) {
                return new Label(name);
            }
            ~Label() = default;

            std::string& name() const { return m_name; }

            int GetId() const { return m_id; }
            int start_address() const { return m_start_address; }
            void set_start_address(int address) { m_start_address = address; }


        private:

            int m_start_address {0};
            int m_id {0};
            std::string& m_name;
        };//class Label

    } // namespace Interpreter
} // namespace js