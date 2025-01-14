cmake_minimum_required(VERSION 3.30)
project(js)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w -g")

find_package(nlohmann_json 3.11.3 REQUIRED)
find_package(CURL REQUIRED)

add_executable(js src/main.cpp
        src/AST.h
        src/AST.cpp
        src/Lexer.h
        src/Lexer.cpp
        src/Parser.cpp
        src/Parser.h
        src/LexerTypes.h
        src/debug.h
        src/EnumTypes.h
        src/Context.h
        src/Util.h
        src/Visitor.h)

target_link_libraries(js PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(js PRIVATE CURL::libcurl)