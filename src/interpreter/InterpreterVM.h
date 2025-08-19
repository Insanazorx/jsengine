#pragma once

namespace js {
    namespace Interpreter {
        class InterpreterVM {
        private:
            InterpreterVM() = default;
            static InterpreterVM* Instance;;
        };
    };

}
