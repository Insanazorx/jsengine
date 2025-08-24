#pragma once

namespace js {
    namespace Interpreter {
        class VM {
        private:
            VM() = default;
            static VM* Instance;;
        };
    };

}
