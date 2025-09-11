#pragma once
#include <string>


namespace js {
    namespace Interpreter {

        class Label : public Instruction {
        private:
            explicit Label(std::string&& name) : m_name(std::move(name)) {next_id = 0;}
        public:
            static Label* Create(std::string&& name) {
                return new Label(std::move(name));
            }
            ~Label() = default;


            int GetId() const { return m_id; }
            void SetId() {m_id = next_id++;}

        private:
            static int next_id;
            int m_id {0};
            const std::string& m_name;
        };//class Label

    } // namespace Interpreter
} // namespace js