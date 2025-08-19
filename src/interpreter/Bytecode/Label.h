#pragma once
#include <string>


namespace js {
    namespace Interpreter {

        class Label {
        private:
            explicit Label(std::string& name) : m_name(name) {next_id = 0;}
        public:
            static Label* Create() {
                return new Label();
            }
            ~Label() = default;


            int GetId() const { return m_id; }
            void SetId() {m_id = next_id++;}

        private:
            static int next_id;
            int m_id {0}; // Default ID is 0
            std::string& m_name;
        };//class Label

    } // namespace Interpreter
} // namespace js