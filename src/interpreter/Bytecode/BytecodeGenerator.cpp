#pragma once
#include "BytecodeGenerator.h"
#include  "BytecodeStream.h"

namespace js {
    namespace Interpreter {
        template <typename... Args> void BytecodeGenerator::BuildCommand(OpCode op, Args... args) {

            BytecodeStream* stream = BytecodeStream::Create();
            *stream << static_cast<BytecodeStream::Bytecode>(op);
            (stream << ... << static_cast<BytecodeStream::Bytecode>(args));


        }
    }
}

