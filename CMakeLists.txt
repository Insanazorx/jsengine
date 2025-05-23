cmake_minimum_required(VERSION 3.30)
project(js)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w -g")

find_package(nlohmann_json 3.11.3 REQUIRED)
find_package(CURL REQUIRED)

add_executable(js src/main.cpp
        src/ast/AST.h
        src/ast/AST.cpp
        src/frontend/Lexer.h
        src/frontend/Lexer.cpp
        src/frontend/Parser.cpp
        src/frontend/Parser.h
        src/frontend/LexerTypes.h
        src/frontend/debug.h
        src/frontend/EnumTypes.h
        src/frontend/Context.h
        src/Util.h
        src/Visitor.h
        src/Validator.h
        src/Validator.cpp
        src/SourceCode.h
        src/SourceCode.cpp
        src/frontend/ErrorOr.h
        src/frontend/Forward.h
        src/ast/ASTNodeFactory.h
        src/interpreter/BytecodeGenerator.h
        src/interpreter/InterpreterVM.h)

target_link_libraries(js PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(js PRIVATE CURL::libcurl)