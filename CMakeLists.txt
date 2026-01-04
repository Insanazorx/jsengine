cmake_minimum_required(VERSION 3.30)
project(js)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w -g")

find_package(nlohmann_json 3.11.3 REQUIRED)
find_package(CURL REQUIRED)

add_executable(js src/main.cpp

        src/Util.h
        src/Visitor.h
        src/interpreter/Bytecode/BytecodeGenerator.h
        src/interpreter/VM/VM.h
        src/interpreter/RuntimeObjects/ExecutionContext.h
        src/interpreter/RuntimeObjects/RuntimeObject.h
        src/interpreter/RuntimeObjects/JSValue.h
        src/interpreter/RuntimeObjects/JSObject.h
        src/interpreter/Bytecode/BytecodeProgram.h
        src/interpreter/Bytecode/Label.h
        src/interpreter/Bytecode/BytecodeGenerator.cpp
        src/interpreter/VM/Register.h
        src/interpreter/VM/VM.cpp
        src/interpreter/VM/ExecutionState.h
        src/interpreter/Bytecode/Bytecodes.h
        src/interpreter/VM/Flag.h
        src/interpreter/RuntimeObjects/Environment.h
        src/interpreter/RuntimeObjects/Binding.h
        src/interpreter/VM/Stack.h
        src/interpreter/RuntimeObjects/BuiltinObjects/ConsoleObject.h
        src/interpreter/RuntimeObjects/BuiltinFunctions/DebugPrint.h
        src/interpreter/RuntimeObjects/JSFunction.h
        src/interpreter/Bytecode/BasicBlock.h
        src/interpreter/Bytecode/Instruction.h
        src/interpreter/Bytecode/BasicBlock.cpp
        src/interpreter/GarbageCollector/Cell.h
        src/interpreter/GarbageCollector/Visitor.h
        src/interpreter/GarbageCollector/Heap.h
        src/interpreter/GarbageCollector/Region.h
        src/interpreter/GarbageCollector/Forward.h
        src/debug.h
)

target_link_libraries(js PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(js PRIVATE CURL::libcurl)