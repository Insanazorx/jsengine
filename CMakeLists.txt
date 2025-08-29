cmake_minimum_required(VERSION 3.30)
project(js)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w -g")

find_package(nlohmann_json 3.11.3 REQUIRED)
find_package(CURL REQUIRED)

add_executable(js src/main.cpp
        src/ast/AST.h
        src/frontend/Lexer.h
        src/frontend/Lexer.cpp
        src/frontend/LexerTypes.h
        src/frontend/debug.h
        src/frontend/EnumTypes.h
        src/Util.h
        src/Visitor.h
        src/Validator.h
        src/Validator.cpp
        src/SourceCode.h
        src/SourceCode.cpp
        src/frontend/ErrorOr.h
        src/frontend/Forward.h
        src/ast/ASTNodeFactory.h
        src/interpreter/Bytecode/BytecodeGenerator.h
        src/interpreter/VM/VM.h
        src/interpreter/RuntimeObjects/ExecutionContext.h
        src/interpreter/RuntimeObjects/RuntimeObject.h
        src/interpreter/RuntimeObjects/Value.h
        src/interpreter/RuntimeObjects/JSObject.h
        src/interpreter/Bytecode/BytecodeStream.h
        src/interpreter/Bytecode/Label.h
        src/interpreter/Bytecode/BytecodeGenerator.cpp
        src/interpreter/VM/Register.h
        src/interpreter/VM/VM.cpp
        src/interpreter/VM/ExecutionState.h
        src/interpreter/Bytecode/Bytecodes.h)

target_link_libraries(js PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(js PRIVATE CURL::libcurl)